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

#include "VMObject.h"
#include "VMInvokable.h"
#include "PrimitiveRoutine.h"

class VMPrimitive: public VMInvokable {
public:
    typedef GCPrimitive Stored;

    static VMPrimitive* GetEmptyPrimitive(VMSymbol* sig, bool classSide);

    VMPrimitive(VMSymbol* sig);

    inline  bool IsEmpty() const;
    inline  void SetRoutine(PrimitiveRoutine* rtn);
    virtual void WalkObjects(walk_heap_fn);
            void SetEmpty(bool value) {empty = value;};
    virtual VMPrimitive* Clone() const;

    virtual void Invoke(Interpreter* interp, VMFrame* frm) {
        routine->Invoke(interp, frm);
    };

    virtual bool IsPrimitive() const {return true;};

    virtual StdString AsDebugString() const;

    std::vector<fomrobject_t*> GetFieldPtrs() {
      std::vector<fomrobject_t*> fields{VMInvokable::GetFieldPtrs()};

      // PrimitiveRoutine's are not allocated via the GC! Therefore this shouldn't be scanned.
//      if (routine) {
//	 fields.push_back((fomrobject_t*) &routine);
//      }

      return fields;
    }
    
private:
    void EmptyRoutine(Interpreter*, VMFrame*);

protected:    
    // protected to be able to access the field in subclass,
    // for instance VMEvaluationPrimitive needs to GC the primitive object
    // hold in the special routine subclass
    PrimitiveRoutine* routine;
private:
    bool empty;

    static const int VMPrimitiveNumberOfFields;

};

bool VMPrimitive::IsEmpty() const {
    return empty;
}

void VMPrimitive::SetRoutine(PrimitiveRoutine* rtn) {
    routine = rtn;
}
