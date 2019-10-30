#include "DispatchTable.h"

std::unique_ptr<CardDealer> CardDealer::dealer {};

CardDealer& CardDealer::instance()
{
   if (!dealer)
      dealer = std::unique_ptr<CardDealer>(new CardDealer());

   return *dealer;
}

template <std::size_t N>
DispatchTable<N> DispatchTable<N>::defaultDispatchTable {};

template <std::size_t N>
VMInvokable*& DispatchTable<N>::operator [](uint8_t index)
{
   return _entries[index];
}

template <std::size_t N>
void DispatchTable<N>::allocDispatchTable(DispatchTable<N>** table)
{
   static uint32_t LAST_UNUSED_TABLE = 0;
   static DispatchTable<N>** TABLE_CACHE[N] = {};

   if (LAST_UNUSED_TABLE < N) {
      TABLE_CACHE[LAST_UNUSED_TABLE++] = table;
      *table = new (GetHeap<HEAP_CLS>()) DispatchTable<N>();
   } else {
      // randomly select a victim class and steal its table!
      auto index = rand() % N;

      *table = *TABLE_CACHE[index];
      *TABLE_CACHE[index] = &DispatchTable<N>::defaultDispatchTable;
      TABLE_CACHE[index] = table;
   }
}

template class DispatchTable<256>;
