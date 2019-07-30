#pragma once

#include "../vmobjects/VMMethod.h"

uint64_t NewCard();

template <std::size_t N>
class DispatchTable
{
public:
  static DispatchTable<N> defaultDispatchTable;

  static void allocDispatchTable(DispatchTable<N>**);  
  VMInvokable*& operator [](uint64_t);

private:
  DispatchTable() {};

  VMInvokable* _entries[N];
  static VMMethod selectorMismatchRaiser;
};

template <std::size_t N>
void allocDispatchTable(DispatchTable<N>**);
