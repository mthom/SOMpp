#include "DispatchTable.h"

uint64_t NewCard() {
   static uint64_t NEW_CARD = 0;
   return NEW_CARD++;
}

template <std::size_t N>
DispatchTable<N> DispatchTable<N>::defaultDispatchTable {};

template <std::size_t N>
VMInvokableStub DispatchTable<N>::selectorMismatchRaiser(NewCard());

template <std::size_t N>
VMInvokable*& DispatchTable<N>::operator [](uint64_t index)
{
   return _entries[index];
}

template <std::size_t N>
void DispatchTable<N>::allocDispatchTable(DispatchTable<N>** table)
{
   static uint8_t LAST_UNUSED_TABLE = 0;
   static DispatchTable<N>** TABLE_CACHE[N];

   if (LAST_UNUSED_TABLE + 1 < N) {
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

template class DispatchTable<256>;

