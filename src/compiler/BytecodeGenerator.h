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

#include <misc/defs.h>
#include <interpreter/bytecodes.h>

#include <vmobjects/ObjectFormats.h>
#include <primitivesCore/PrimitiveContainer.h>

#include "MethodGenerationContext.h"

class BytecodeGenerator {
public:
    void EmitHALT(MethodGenerationContext* mgenc);
    void EmitDUP(MethodGenerationContext* mgenc);
    void EmitPUSHLOCAL(MethodGenerationContext* mgenc,    long idx, int ctx);
    void EmitPUSHARGUMENT(MethodGenerationContext* mgenc, long idx, int ctx);
    void EmitPUSHFIELD(MethodGenerationContext* mgenc, VMSymbol* field);
    void EmitPUSHBLOCK(MethodGenerationContext* mgenc, VMMethod* block);
    void EmitPUSHCONSTANT(MethodGenerationContext* mgenc, vm_oop_t cst);
    void EmitPUSHCONSTANTString(MethodGenerationContext* mgenc, VMString* str);
    void EmitPUSHGLOBAL(MethodGenerationContext* mgenc, VMSymbol* global);
    void EmitPOP(MethodGenerationContext* mgenc);
    void EmitPOPLOCAL(MethodGenerationContext* mgenc,    long idx, int ctx);
    void EmitPOPARGUMENT(MethodGenerationContext* mgenc, long idx, int ctx);
    void EmitPOPFIELD(MethodGenerationContext* mgenc, VMSymbol* field);
    void EmitSEND(MethodGenerationContext* mgenc, VMSymbol* msg);
    void EmitSUPERSEND(MethodGenerationContext* mgenc, VMSymbol* msg);
    void EmitCard(MethodGenerationContext* mgenc, uint64_t card);
    void EmitRETURNLOCAL(MethodGenerationContext* mgenc);
    void EmitRETURNNONLOCAL(MethodGenerationContext* mgenc);

    size_t EmitJUMP_IF_FALSE(MethodGenerationContext* mgenc);
    size_t EmitJUMP_IF_TRUE(MethodGenerationContext* mgenc);
    size_t EmitJUMP(MethodGenerationContext* mgenc);
};
