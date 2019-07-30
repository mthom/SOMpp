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

#include "DispatchTable.h"

#include "VMInvokable.h"
#include "VMSymbol.h"
#include "VMClass.h"

VMInvokable::VMInvokable(long nof)
  : VMObject(nof + 2)
  , card(newCard())
{}

bool VMInvokable::IsPrimitive() const {
    return false;
}

VMSymbol* VMInvokable::GetSignature() const {
    return load_ptr(signature);
}

void VMInvokable::SetSignature(VMSymbol* sig) {
    store_ptr(signature, sig);
}

void VMInvokable::WalkObjects(walk_heap_fn walk) {
    clazz     = static_cast<GCClass*>(walk(clazz));
    signature = static_cast<GCSymbol*>(walk(signature));
    if (holder) {
        holder = static_cast<GCClass*>(walk(holder));
    }
}

VMClass* VMInvokable::GetHolder() const {
    return load_ptr(holder);
}

void VMInvokable::SetHolder(VMClass* hld) {
    store_ptr(holder, hld);
}

uint8_t VMInvokable::GetSelectorCode(uint64_t card)
{
   static uint8_t LAST_CODE = 0;
   
   if ((auto it = cardCodeMap.find(card)) != cardCodeMap.end()) {
      if (codeCardMap[it->second] != card) {
	// must have LAST_CODE == 255 to get here.
	
	// randomly select a code to be stolen.
	// srand(time(0));
	uint8_t code = (uint8_t) (rand() % 256);
	
	it->second = code;
	codeCardMap[code] = card;
	
	return code;
      } else {
         return it->second;
      }
   } else {
      uint8_t code = LAST_CODE < 255 ? LAST_CODE++ : (uint8_t) (rand() % 256);

      codeCardMap[code] = card;
      cardCodeMap[card] = code;

      return code;
   }
}
