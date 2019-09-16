/*
 * AbstractVMObject.cpp
 *
 *  Created on: 10.03.2011
 *      Author: christian
 */

#include "AbstractObject.h"

#include <vm/Universe.h>

#include "VMFrame.h"
#include "VMClass.h"
#include "VMInvokable.h"

#include <algorithm>
#include <numeric>

size_t AbstractVMObject::GetHash() {
    return (size_t) this;
}

// returns true if the invokable is compiled, meaning that invocation is immediate.
bool AbstractVMObject::Send(Interpreter* interp, StdString selectorString, vm_oop_t* arguments, long argc) {
    VMFrame* frame = interp->GetFrame();
    VMSymbol* selector = GetUniverse()->SymbolFor(selectorString);
    frame->Push(this);

    for (long i = 0; i < argc; ++i) {
        frame->Push(arguments[i]);
    }

    VMClass* cl = GetClass();
    VMInvokable* invokable = cl->LookupInvokable(selector);
    invokable->Invoke(interp, frame);

    return invokable->HasCompiledMethod();
}

long AbstractVMObject::GetFieldIndex(VMSymbol* fieldName) const {
    return GetClass()->LookupFieldIndex(fieldName);
}
