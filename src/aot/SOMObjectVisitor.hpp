#pragma once

#include <set>

#include "../vmobjects/ObjectFormats.h"
#include "BufferWriter.hpp"

#include "../misc/defs.h"

class VMClass;
class VMObject;
class VMArray;
class VMBlock;
class VMDouble;
class VMInteger;
class VMInvokable;
class VMPrimitive;
class VMEvaluationPrimitive;
class VMFrame;
class VMMethod;
class VMString;
class VMSymbol;

class SOMObjectVisitor {
public:
    virtual void operator()(VMClass*) = 0;
    virtual void operator()(VMObject*, long numberOfNonIntrinsicFields = 0) = 0;
    virtual void operator()(VMArray*) = 0;
    virtual void operator()(VMBlock*) = 0;
    virtual void operator()(VMDouble*) = 0;
    virtual void operator()(VMInteger*) = 0;
    virtual void operator()(VMInvokable*) = 0;
    virtual void operator()(VMPrimitive*) = 0;
    virtual void operator()(VMEvaluationPrimitive*) = 0;
    virtual void operator()(VMFrame*) = 0;
    virtual void operator()(VMMethod*) = 0;
    virtual void operator()(VMString*) = 0;
    virtual void operator()(VMSymbol*) = 0;
};
