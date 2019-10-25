#pragma once

#include <memory>

#include "../vmobjects/VMMethod.h"
#include "../vmobjects/VMObjectBase.h"

class CardDealer
{
private:
   uint64_t card;
   static std::unique_ptr<CardDealer> dealer;

   CardDealer() : card(0) {}

public:
   static CardDealer& instance();

   static uint64_t GetNewCard(const char*) {
      return instance().card++;
   }

   static uint64_t GetCurrentCard() {
      return instance().card;
   }

   static void SetCard(uint64_t newCard) {
      if (newCard > instance().card) {
         instance().card = newCard;
      }
   }
};

template <std::size_t N>
class DispatchTable : public VMObjectBase
{
public:
   static DispatchTable<N> defaultDispatchTable;

   static void allocDispatchTable(DispatchTable<N>**);

   VMInvokable*& operator [](uint8_t);

   size_t GetObjectSize() const {
     return sizeof(DispatchTable<N>);
   }
  
   std::vector<fomrobject_t*> GetFieldPtrs() {
     std::vector<fomrobject_t*> fields;
     fields.reserve(256);

     for(int i = 0; i < 256; ++i) {
       if (_entries[i] && _entries[i] != nullptr) {//&selectorMismatchRaiser) {
	 fields.push_back((fomrobject_t*) &_entries[i]);
       }
     }

     return fields;
   }

   void* operator new(size_t numBytes, HEAP_CLS* heap)
   {
     // if outsideNursery flag is set or object is too big for nursery, we
     // allocate a mature object
     void* result;
#if GC_TYPE==GENERATIONAL
     if (outsideNursery) {
       result = (void*) heap->AllocateMatureObject(numBytes);
     } else {
       result = (void*) heap->AllocateNurseryObject(numBytes);
     }
#else
     result = (void*) heap->AllocateDispatchTable(numBytes);
#endif

     assert(result != INVALID_VM_POINTER);
     return result;
   }

   private:
   DispatchTable(): _entries{} {
     gcfield = 0;
   }

   VMInvokable* _entries[N];
};
