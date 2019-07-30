#include "DispatchTable.h"
#include "VMFrame.h"
#include "Interpreter.h"

uint64_t newCard() {
   static uint64_t NEW_CARD = 0;
   return NEW_CARD++;
}

DispatchTableEntry::DispatchTableEntry()
  : code{0}
  , method{&DispatchTable::selectorMismatchRaiser}
{}

template <std::size_t N>
DispatchTableEntry DispatchTable<N>::operator [](uint64_t index)
{
   return _entries[index];
}

template <std::size_t N>
void allocDispatchTable(DispatchTable<N>** table)
{
   static uint8_t LAST_UNUSED_TABLE = 0;
   static DispatchTable<N>** TABLE_CACHE[N];

   if (LAST_UNUSED_TABLE < N) {
      TABLE_CACHE[LAST_UNUSED_TABLE++] = table;
      *table = new DispatchTable<N>();
   } else {
      // randomly select a victim class and steal its table!
      auto index = rand() % N;
      
      *table = *TABLE_CACHE[index];
      *TABLE_CACHE[index] = &DispatchTable<N>::defaultDispatchTable;
      TABLE_CACHE[index] = table;
   }
}
