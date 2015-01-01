#pragma once

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

#include <vector>

#include "VMObject.h"
#include "VMSymbol.h"
#include "VMArray.h"

#include <misc/defs.h>

#if defined(_MSC_VER)   //Visual Studio
#include <windows.h> 
#include "../primitives/Core.h"
#endif

class ClassGenerationContext;

class VMClass: public VMObject {
public:
    VMClass();
    VMClass(long numberOfFields);

    inline pVMClass     GetSuperClass() const;
    inline void         SetSuperClass(pVMClass);
    inline bool         HasSuperClass() const;
    inline pVMSymbol    GetName() const;
    inline void         SetName(pVMSymbol);
    inline VMArray*     GetInstanceFields() const;
    inline void         SetInstanceFields(VMArray*);
    inline VMArray*     GetInstanceInvokables() const;
           void         SetInstanceInvokables(VMArray*);
           long         GetNumberOfInstanceInvokables() const;
           pVMInvokable GetInstanceInvokable(long) const;
           void         SetInstanceInvokable(long, pVMObject);
           pVMInvokable LookupInvokable(pVMSymbol) const;
           long         LookupFieldIndex(pVMSymbol) const;
           bool         AddInstanceInvokable(pVMObject);
           void         AddInstancePrimitive(pVMPrimitive);
           pVMSymbol    GetInstanceFieldName(long)const;
           long         GetNumberOfInstanceFields() const;
           bool         HasPrimitives() const;
           void         LoadPrimitives(const vector<StdString>&);
    virtual pVMClass    Clone() const;
           void         WalkObjects(oop_t (*walk)(oop_t));
    
    virtual void MarkObjectAsInvalid();

private:
    StdString genLoadstring(const StdString& cp,
            const StdString& cname
    ) const;

    StdString genCoreLoadstring(const StdString& cp) const;

    void* loadLib(const StdString& path) const;
    bool isResponsible(void* handle, const StdString& cl) const;
    void setPrimitives(void* handle, const StdString& cname);
    long numberOfSuperInstanceFields() const;

    pVMClass superClass;
    pVMSymbol name;
    VMArray* instanceFields;
    VMArray* instanceInvokables;

    static const long VMClassNumberOfFields;
};

pVMClass VMClass::GetSuperClass() const {
    return superClass;
}

void VMClass::SetSuperClass(pVMClass sup) {
    superClass = sup;
    write_barrier(this, sup);
}

pVMSymbol VMClass::GetName() const {
    return name;
}

void VMClass::SetName(pVMSymbol nam) {
    name = nam;
    write_barrier(this, nam);
}

bool VMClass::HasSuperClass() const {
    assert(Universe::IsValidObject(superClass));
    return superClass != nilObject;
}

VMArray* VMClass::GetInstanceFields() const {
    return instanceFields;
}

void VMClass::SetInstanceFields(VMArray* instFields) {
    instanceFields = instFields;
    write_barrier(this, instFields);
}

VMArray* VMClass::GetInstanceInvokables() const {
    return instanceInvokables;
}
