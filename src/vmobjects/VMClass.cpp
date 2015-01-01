/*
 *
 *
 Copyright (c) 2007 Michael Haupt, Tobias Pape, Arne Bergmann
 Software Architecture Group, Hasso Plattner Institute, Potsdam, Germany
 http://www.hpi.uni-potsdam.de/swa/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "VMClass.h"
#include "VMArray.h"
#include "VMSymbol.h"
#include "VMInvokable.h"
#include "VMPrimitive.h"
#include "PrimitiveRoutine.h"

#include <fstream>
#include <typeinfo>

#if defined(__GNUC__)
#   include <dlfcn.h>
#else   //Visual Studio
/**
 * Emualting the dl-interface with win32 means
 */
#   define WIN32_LEAN_AND_MEAN
#   define dlerror()   "Load Error"
#   define dlsym       GetProcAddress
#   define DL_LOADMODE nullptr, LOAD_WITH_ALTERED_SEARCH_PATH
#   define dlopen      LoadLibrary
#   define dlclose     FreeLibrary
//#   include <windows.h> //included in VMClass.h if necessary

#endif

/*
 * Format definitions for Primitive naming scheme.
 *
 */
#define CLASS_METHOD_FORMAT_S "%s::%s"
// as in AClass::aClassMethod
#define INSTANCE_METHOD_FORMAT_S "%s::%s_"
// as in AClass::anInstanceMethod_

const long VMClass::VMClassNumberOfFields = 4;

VMClass::VMClass() :
        VMObject(VMClassNumberOfFields), superClass(nullptr), name(nullptr), instanceFields(
                nullptr), instanceInvokables(nullptr) {
}

pVMClass VMClass::Clone() const {
    pVMClass clone = new (GetHeap<HEAP_CLS>(), objectSize - sizeof(VMClass) ALLOC_MATURE) VMClass(*this);
    memcpy(SHIFTED_PTR(clone,sizeof(VMObject)),
            SHIFTED_PTR(this,sizeof(VMObject)), GetObjectSize() -
            sizeof(VMObject));
    return clone;
}

VMClass::VMClass(long numberOfFields) :
        VMObject(numberOfFields + VMClassNumberOfFields) {
}

void VMClass::WalkObjects(oop_t (*walk)(oop_t)) {
    clazz = static_cast<pVMClass>(walk(clazz));
    if (superClass)
        superClass = static_cast<pVMClass>(walk(superClass));
    name = static_cast<pVMSymbol>(walk(name));
    instanceFields = static_cast<VMArray*>(walk(instanceFields));
    instanceInvokables = static_cast<VMArray*>(walk(instanceInvokables));

    oop_t* fields = FIELDS;

    for (long i = VMClassNumberOfFields + 0/*VMObjectNumberOfFields*/; i < numberOfFields; i++)
        fields[i] = walk(fields[i]);
}

void VMClass::MarkObjectAsInvalid() {
    superClass         = (pVMClass)  INVALID_POINTER;
    name               = (pVMSymbol) INVALID_POINTER;
    instanceFields     = (VMArray*)  INVALID_POINTER;
    instanceInvokables = (VMArray*)  INVALID_POINTER;
}

bool VMClass::AddInstanceInvokable(pVMObject ptr) {
    pVMInvokable newInvokable = static_cast<pVMInvokable>(ptr);
    if (newInvokable == nullptr) {
        GetUniverse()->ErrorExit("Error: trying to add non-invokable to invokables array");
        return false;
    }
    //Check whether an invokable with the same signature exists and replace it if that's the case
    long numIndexableFields = instanceInvokables->GetNumberOfIndexableFields();
    for (long i = 0; i < numIndexableFields; ++i) {
        pVMInvokable inv = static_cast<pVMInvokable>(instanceInvokables->GetIndexableField(i));
        if (inv != nullptr) {
            if (newInvokable->GetSignature() == inv->GetSignature()) {
                SetInstanceInvokable(i, ptr);
                return false;
            }
        } else {
            GetUniverse()->ErrorExit("Invokables array corrupted. "
                                     "Either NULL pointer added or pointer to non-invokable.");
            return false;
        }
    }
    //it's a new invokable so we need to expand the invokables array.
    instanceInvokables = instanceInvokables->CopyAndExtendWith(ptr);
    write_barrier(this, instanceInvokables);

    return true;
}

void VMClass::AddInstancePrimitive(pVMPrimitive ptr) {
    if (AddInstanceInvokable(ptr)) {
        //cout << "Warn: Primitive "<<ptr->GetSignature<<" is not in class definition for class " << name->GetStdString() << endl;
    }
}

pVMSymbol VMClass::GetInstanceFieldName(long index) const {
    long numSuperInstanceFields = numberOfSuperInstanceFields();
    if (index >= numSuperInstanceFields) {
        index -= numSuperInstanceFields;
        return static_cast<pVMSymbol>(instanceFields->GetIndexableField(index));
    }
    return superClass->GetInstanceFieldName(index);
}

void VMClass::SetInstanceInvokables(VMArray* invokables) {
    instanceInvokables = invokables;
    write_barrier(this, invokables);

    long numInvokables = GetNumberOfInstanceInvokables();
    for (long i = 0; i < numInvokables; ++i) {
        oop_t invo = instanceInvokables->GetIndexableField(i);
        //check for Nil object
        if (invo != nilObject) {
            //not Nil, so this actually is an invokable
            pVMInvokable inv = static_cast<pVMInvokable>(invo);
            inv->SetHolder(this);
        }
    }
}

long VMClass::GetNumberOfInstanceInvokables() const {
    return instanceInvokables->GetNumberOfIndexableFields();
}

pVMInvokable VMClass::GetInstanceInvokable(long index) const {
    return static_cast<pVMInvokable>(instanceInvokables->GetIndexableField(index));
}

void VMClass::SetInstanceInvokable(long index, pVMObject invokable) {
    instanceInvokables->SetIndexableField(index, invokable);
    if (invokable != nilObject) {
        pVMInvokable inv = static_cast<pVMInvokable>( invokable );
        inv->SetHolder(this);
    }
}

pVMInvokable VMClass::LookupInvokable(pVMSymbol name) const {
    assert(Universe::IsValidObject((oop_t) this));
    
    pVMInvokable invokable = name->GetCachedInvokable(this);
    if (invokable != nullptr)
        return invokable;

    long numInvokables = GetNumberOfInstanceInvokables();
    for (long i = 0; i < numInvokables; ++i) {
        invokable = GetInstanceInvokable(i);
        if (invokable->GetSignature() == name) {
            name->UpdateCachedInvokable(this, invokable);
            return invokable;
        }
    }

    // look in super class
    if (HasSuperClass()) {
        return superClass->LookupInvokable(name);
    }
    
    // invokable not found
    return nullptr;
}

long VMClass::LookupFieldIndex(pVMSymbol name) const {
    long numInstanceFields = GetNumberOfInstanceFields();
    for (long i = 0; i <= numInstanceFields; ++i) {
        // even with GetNumberOfInstanceFields == 0 there is the class field
        if (name == GetInstanceFieldName(i)) {
            return i;
        }
    }
    return -1;
}

long VMClass::GetNumberOfInstanceFields() const {
    return instanceFields->GetNumberOfIndexableFields()
            + numberOfSuperInstanceFields();
}

bool VMClass::HasPrimitives() const {
    long numInvokables = GetNumberOfInstanceInvokables();
    for (long i = 0; i < numInvokables; ++i) {
        pVMInvokable invokable = GetInstanceInvokable(i);
        if (invokable->IsPrimitive()) return true;
    }
    return false;
}

void VMClass::LoadPrimitives(const vector<StdString>& cp) {
    // the library handle
    void* dlhandle = nullptr;

    // cached object properties
    StdString cname = name->GetStdString();

#if defined (__GNUC__)
    //// iterate the classpathes
    for (vector<StdString>::const_iterator i = cp.begin();
            (i != cp.end()) && dlhandle == nullptr; ++i) {
        // check the core library
        StdString loadstring = genCoreLoadstring(*i);
        dlhandle = loadLib(loadstring);
        if (dlhandle != nullptr) {

            if (isResponsible(dlhandle, cname))
                // the core library is found and responsible
                break;
        }

        // the core library is not found or not responsible, 
        // continue w/ class file
        loadstring = genLoadstring(*i, cname);
        cout << loadstring.c_str() << endl;
        dlhandle = loadLib(loadstring);
        if (dlhandle != nullptr) {
            //
            // the class library was found...
            //
            if (isResponsible(dlhandle, cname)) {
                //
                // ...and is responsible.
                //
                break;
            } else {
                //
                // ... but says not responsible, but we have to
                // close it nevertheless
                //
                dlclose(dlhandle);
                GetUniverse()->ErrorExit("Library claims no resonsibility, but musn't!");
            }

        }
        /*
         * continue checking the next class path
         */
    }

    // finished cycling,
    // check if a lib was found.
    if (dlhandle == nullptr) {
        cout << "load failure: ";
        cout << "could not load primitive library for " << cname << endl;
        GetUniverse()->Quit(ERR_FAIL);
    }

#endif
    ///*
    // * do the actual loading for both class and metaclass
    // *
    // */
    setPrimitives(dlhandle, cname);
    GetClass()->setPrimitives(dlhandle, cname);
}

long VMClass::numberOfSuperInstanceFields() const {
    if (HasSuperClass())
        return superClass->GetNumberOfInstanceFields();
    return 0;
}

//LoadPrimitives helper
#define sharedExtension ".csp"

StdString VMClass::genLoadstring(const StdString& cp,
        const StdString& cname) const {

    StdString loadstring = string(cp);
    loadstring += fileSeparator;
    loadstring += cname;
    loadstring += sharedExtension;

    return loadstring;
}

/**
 *  generate the string containing the path to a SOMCore which may be located
 *  at the classpath given.
 *
 */
StdString VMClass::genCoreLoadstring(const StdString& cp) const {
#define S_CORE "SOMCore"
    StdString corename = string(S_CORE);
    StdString result = genLoadstring(cp, corename);

    return result;
}

/**
 * load the given library, return the handle
 *
 */
void* VMClass::loadLib(const StdString& path) const {
#ifdef __DEBUG
    cout << "loadLib " << path << endl;
#endif
#if defined(__GNUC__)
#if DEBUG
#define    DL_LOADMODE RTLD_NOW
#else
#define    DL_LOADMODE RTLD_LAZY
#endif

    // static handle. will be returned
    void* dlhandle;
    // try load lib
    if ((dlhandle = dlopen(path.c_str(), DL_LOADMODE))) {
        //found.
        return dlhandle;
    } else {
        cout << "Error loading library " << path << ": " << dlerror() << endl;
        return nullptr;
    }
#else
    return nullptr;
#endif

}

/**
 * check, whether the lib referenced by the handle supports the class given
 *
 */
bool VMClass::isResponsible(void* dlhandle, const StdString& cl) const {
#if defined(__GNUC__)
    // function handler
    SupportsClass* supports_class = nullptr;

    supports_class = (SupportsClass*) dlsym(dlhandle, "supportsClass");
    if (!supports_class) {
        cout << "error: " << dlerror() << endl;
        GetUniverse()->ErrorExit("Library doesn't have expected format: ");
        return false;
    }

    // test class responsibility
    return supports_class(cl.c_str());
#else 
    return true;
#endif
}

/*
 * set the routines for primitive marked invokables of the given class
 *
 */
void VMClass::setPrimitives(void* dlhandle, const StdString& cname) {
    pVMPrimitive thePrimitive;
    PrimitiveRoutine* routine = nullptr;
    pVMInvokable anInvokable;
    // iterate invokables
    long numInvokables = GetNumberOfInstanceInvokables();
    for (long i = 0; i < numInvokables; i++) {
        anInvokable = GetInstanceInvokable(i);
#ifdef __DEBUG
        cout << "cname: >" << cname << "<"<< endl;
        cout << anInvokable->GetSignature()->GetStdString() << endl;
#endif
        if(anInvokable->IsPrimitive()) {
#ifdef __DEBUG
            cout << "... is a primitive, and is going to be loaded now" << endl;
#endif
            thePrimitive = static_cast<pVMPrimitive>( anInvokable );
            //
            // we have a primitive to load
            // get it's selector
            //
            pVMSymbol sig = thePrimitive->GetSignature();
            StdString selector = sig->GetPlainString();
#if defined(__GNUC__)
            CreatePrimitive* create =
            (CreatePrimitive*) dlsym(dlhandle, "create");
#endif
            routine = create(cname, selector);

            if(!routine) {
                cout << "could not load primitive '"<< selector
                <<"' for class " << cname << endl;

                GetUniverse()->Quit(ERR_FAIL);
            }

            // set routine
            thePrimitive->SetRoutine(routine);
            thePrimitive->SetEmpty(false);
        }
#ifdef __DEBUG
        else {
            cout << "... is not a primitive" << endl;
        }
#endif
    }
}

