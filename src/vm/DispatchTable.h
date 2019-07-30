#pragma once

#include "../vmobjects/VMInvokable.h"

uint64_t newCard();

struct DispatchTableEntry {
  DispatchTableEntry();
  
  uint8_t code;
  VMInvokable* method;
};

template <std::size_t N>
class DispatchTable
{
friend class DispatchTableEntry;
   
public:
  static DispatchTable<N> defaultDispatchTable;
  DispatchTableEntry operator [](uint64_t);

private:
  DispatchTable() {};

  DispatchTableEntry _entries[N];
  static VMInvokable selectorMismatchRaiser;
};

template <std::size_t N>
void allocDispatchTable(DispatchTable<N>**);
