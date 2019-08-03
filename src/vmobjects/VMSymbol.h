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

#include <iostream>

#include "VMString.h"
#include "VMObject.h"

class VMSymbol: public VMString {
    friend class SOMppMethod;
public:
    typedef GCSymbol Stored;

    VMSymbol(const char* str);
    VMSymbol(const StdString& s);
    virtual StdString GetPlainString() const;
    virtual size_t GetObjectSize() const;
    virtual VMSymbol* Clone() const;
    virtual VMClass* GetClass() const;

    virtual StdString AsDebugString() const;

    std::vector<fomrobject_t*> GetFieldPtrs() {
      std::vector<fomrobject_t*> fields;

      fields.push_back((fomrobject_t*) &cachedClass_invokable[0]);
      fields.push_back((fomrobject_t*) &cachedClass_invokable[1]);
      fields.push_back((fomrobject_t*) &cachedClass_invokable[2]);
      fields.push_back((fomrobject_t*) &cachedInvokable[0]);
      fields.push_back((fomrobject_t*) &cachedInvokable[1]);      
      fields.push_back((fomrobject_t*) &cachedInvokable[2]);

      return fields;
    }

    uint64_t GetCard() const {
      return card;
    }

private:
    const int numberOfArgumentsOfSignature;
    const GCClass* cachedClass_invokable[3];
    long nextCachePos;
    GCInvokable* cachedInvokable[3];
    const uint64_t card;
    
    inline VMInvokable* GetCachedInvokable(const VMClass*) const;
    inline void UpdateCachedInvokable(const VMClass* cls, VMInvokable* invo);

    virtual void WalkObjects(walk_heap_fn);

    friend class Signature;
    friend class VMClass;
};


#include "VMClass.h"
#include "VMInvokable.h"


VMInvokable* VMSymbol::GetCachedInvokable(const VMClass* cls) const {
    if (cls == load_ptr(cachedClass_invokable[0]))
        return load_ptr(cachedInvokable[0]);
    else if (cls == load_ptr(cachedClass_invokable[1]))
        return load_ptr(cachedInvokable[1]);
    else if (cls == load_ptr(cachedClass_invokable[2]))
        return load_ptr(cachedInvokable[2]);
    return nullptr;
}

void VMSymbol::UpdateCachedInvokable(const VMClass* cls, VMInvokable* invo) {
    store_ptr(cachedInvokable[nextCachePos], invo);
    store_ptr(cachedClass_invokable[nextCachePos], const_cast<VMClass*>(cls));
    nextCachePos = (nextCachePos + 1) % 3;
}
