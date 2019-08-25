#include "../vmobjects/AbstractObject.h"
#include "../vmobjects/VMObject.h"
#include "../vmobjects/VMArray.h"
#include "../vmobjects/VMInteger.h"
#include "../vmobjects/VMBlock.h"
#include "../vmobjects/VMClass.h"
#include "../vmobjects/VMDouble.h"
#include "../vmobjects/VMEvaluationPrimitive.h"
#include "../vmobjects/VMFrame.h"
#include "../vmobjects/VMMethod.h"
#include "../vmobjects/VMPrimitive.h"
#include "../vmobjects/VMString.h"
#include "../vmobjects/VMSymbol.h"

#include "../vm/Universe.h"

#include "ObjectDeserializer.hpp"

template <typename T>
inline void read(T* sink, void* source)
{
   using unconst_T = std::remove_const_t<T>;
   *const_cast<unconst_T*>(sink) = *reinterpret_cast<T*>(source);
}

template <typename T>
inline void read_direct(T* sink, T source)
{
   using unconst_T = std::remove_const_t<T>;
   *const_cast<unconst_T*>(sink) = source;
}

gc_oop_t ObjectDeserializer::operator()(MetadataIterator& it)
{
   ItemHeader header = it.header();

   switch (header.desc)
   {
   case ItemHeader::object:
     return createObject(it);
   case ItemHeader::array:
     return createArray(it);
   case ItemHeader::block:
     return createBlock(it);
   case ItemHeader::clazz:
     return createClass(it);
   case ItemHeader::num_integer:
     return createInteger(it);
   case ItemHeader::num_double:
     return createDouble(it);
   case ItemHeader::eval_prim:
     return createEvaluationPrimitive(it);
   case ItemHeader::frame:
     return nullptr; //createFrame(it);
   case ItemHeader::method:
     return createMethod(it);
   case ItemHeader::prim:
     return createPrimitive(it);
   case ItemHeader::vm_string:
     return createString(it);
   case ItemHeader::symbol:
     return createSymbol(it);
   }
}

gc_oop_t ObjectDeserializer::isSeenObject(const ItemHeader& header)
{
   if (auto it = oldNewAddresses.find(header); it != oldNewAddresses.end()) {
     return _store_ptr(it->second);
   }

   return nullptr;
}

GCObject* ObjectDeserializer::createObject(MetadataIterator& it, long numberOfIntrinsicFields)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCObject*>(obj);
   }

   long additionalBytes = header.size - sizeof(VMObject);
   long numberOfFields  = additionalBytes / sizeof(gc_oop_t);

   VMObject* obj = new (GetHeap<HEAP_CLS>(), additionalBytes) VMObject(numberOfFields);

   oldNewAddresses[header] = obj;

   read(&obj->hash, *it);
   it += sizeof(obj->hash);

   read(&obj->objectSize, *it);
   it += sizeof(obj->objectSize);

   read(&obj->numberOfFields, *it);
   it += sizeof(obj->numberOfFields);

   read(&obj->fieldsOffset, *it);
   it += sizeof(obj->fieldsOffset);

   read_direct(&obj->clazz, createClass(it));

   for(long i = numberOfIntrinsicFields; i < obj->GetNumberOfFields(); ++i) {
      auto offset = reinterpret_cast<GCObject**>(&obj->clazz + obj->fieldsOffset + i);
      read_direct(offset, static_cast<GCObject*>((*this)(it)));
   }

   return _store_ptr(obj);
}

GCClass* ObjectDeserializer::createClass(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCClass*>(obj);
   }

   long additionalBytes = header.size - sizeof(VMClass);
   long numberOfFields  = additionalBytes / sizeof(gc_oop_t);

   VMClass* clazz = new (GetHeap<HEAP_CLS>(), additionalBytes) VMClass(numberOfFields);
   oldNewAddresses[header] = clazz;

   clazz->AssignObject(*load_ptr(createObject(it, VMClass::VMClassNumberOfFields)),
		       VMClass::VMClassNumberOfFields);

   read_direct(&clazz->superClass, createClass(it));
   read_direct(&clazz->name, createSymbol(it));
   
   read_direct(&clazz->instanceFields, createArray(it));
   read_direct(&clazz->instanceInvokables, createArray(it));

   return _store_ptr(clazz);
}

GCArray* ObjectDeserializer::createArray(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCArray*>(obj);
   }

   VMObject* obj = load_ptr(createObject(it, VMArray::VMArrayNumberOfFields));

   long additionalBytes = header.size - sizeof(VMArray);
   long numberOfFields  = obj->numberOfFields - VMArray::VMArrayNumberOfFields;
   long numberOfArrayFields = additionalBytes / sizeof(gc_oop_t) - obj->numberOfFields;

   VMArray* array = new (GetHeap<HEAP_CLS>(), additionalBytes)
     VMArray(numberOfArrayFields, numberOfFields);

   oldNewAddresses[header] = array;

   array->AssignObject(*obj, VMArray::VMArrayNumberOfFields);

   for(long i = 0; i < array->GetNumberOfIndexableFields(); ++i) {
      GCObject** offset = reinterpret_cast<GCObject**>(&array->clazz); 
      offset += array->fieldsOffset + array->GetNumberOfFields() + i;

      read_direct(offset, static_cast<GCObject*>((*this)(it)));
   }

   return _store_ptr(array);
}

GCBlock* ObjectDeserializer::createBlock(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCBlock*>(obj);
   }

   auto obj = dynamic_cast<VMObject*>(load_ptr((*this)(it)));

   long additionalBytes = header.size - sizeof(VMBlock);
   VMBlock* block = new (GetHeap<HEAP_CLS>(), additionalBytes) VMBlock();

   oldNewAddresses[header] = block;

   return _store_ptr(block);
}

GCDouble* ObjectDeserializer::createDouble(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCDouble*>(obj);
   }

   VMDouble* dbl = new (GetHeap<HEAP_CLS>()) VMDouble(0);

   oldNewAddresses[header] = dbl;

   read(&dbl->embeddedDouble, *it);
   it += sizeof(dbl->embeddedDouble);

   return _store_ptr(dbl);
}

GCInteger* ObjectDeserializer::createInteger(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);
   
   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCInteger*>(obj);
   }

   VMInteger* integer = new (GetHeap<HEAP_CLS>()) VMInteger(0);

   oldNewAddresses[header] = integer;

   read(&integer->embeddedInteger, *it);
   it += sizeof(integer->embeddedInteger);

   return _store_ptr(integer);
}

GCPrimitive* ObjectDeserializer::createPrimitive(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCPrimitive*>(obj);
   }

   long numberOfIntrinsicFields = VMPrimitive::VMPrimitiveNumberOfFields + VMInvokable::VMInvokableNumberOfFields;
   VMObject* obj = load_ptr(createObject(it, numberOfIntrinsicFields));

   // this is the location of the signature.
   VMSymbol* signature = load_ptr(createSymbol(it));

   long additionalBytes = header.size - sizeof(VMPrimitive);
   VMPrimitive* prim = new (GetHeap<HEAP_CLS>(), additionalBytes) VMPrimitive(signature);

   oldNewAddresses[header] = prim;

   prim->AssignObject(*obj, numberOfIntrinsicFields);

   read_direct(&prim->holder, createClass(it));

   read(&prim->empty, *it);
   it += sizeof(prim->empty);

   return _store_ptr(prim);
}

GCEvaluationPrimitive* ObjectDeserializer::createEvaluationPrimitive(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCEvaluationPrimitive*>(obj);
   }

   long numberOfIntrinsicFields = VMPrimitive::VMPrimitiveNumberOfFields + VMInvokable::VMInvokableNumberOfFields
     + VMEvaluationPrimitive::VMEvaluationPrimitiveNumberOfFields;
   VMObject* obj = load_ptr(createObject(it, numberOfIntrinsicFields));

   // this is the location of the signature.
   VMSymbol* signature = load_ptr(createSymbol(it));
   long argc = 0;

   read(&argc, *it);
   it += sizeof(argc);
   
   long additionalBytes = header.size - sizeof(VMEvaluationPrimitive);
   VMEvaluationPrimitive* prim = new (GetHeap<HEAP_CLS>(), additionalBytes) VMEvaluationPrimitive(signature, argc);

   oldNewAddresses[header] = prim;

   prim->AssignObject(*obj, numberOfIntrinsicFields);

   read_direct(&prim->holder, createClass(it));

   read(&prim->empty, *it);
   it += sizeof(prim->empty);

   return _store_ptr(prim);
}

GCMethod* ObjectDeserializer::createMethod(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCMethod*>(obj);
   }

   long numberOfIntrinsicFields = VMMethod::VMMethodNumberOfFields + VMInvokable::VMInvokableNumberOfFields;
   VMObject* obj = load_ptr(createObject(it, numberOfIntrinsicFields));

   long additionalBytes = header.size - sizeof(VMMethod);
   long numberOfConstantsEmbedded = 0;

   read(&numberOfConstantsEmbedded, *it);
   it += sizeof(numberOfConstantsEmbedded);

   VMInteger* bcLength = load_ptr(createInteger(it));
   long numberOfFields = obj->numberOfFields - numberOfIntrinsicFields;

   VMMethod* method = new (GetHeap<HEAP_CLS>(), additionalBytes)
     VMMethod(bcLength->GetEmbeddedInteger(), numberOfConstantsEmbedded, numberOfFields);

   oldNewAddresses[header] = method;

   method->AssignObject(*obj, numberOfIntrinsicFields);

   read_direct(&method->numberOfLocals, (*this)(it));
   read_direct(&method->maximumNumberOfStackElements, (*this)(it));
   read_direct(&method->numberOfArguments, (*this)(it));
   read_direct(&method->numberOfConstants, (*this)(it));

   read_direct(&method->signature, createSymbol(it));
   read_direct(&method->holder, createClass(it));

   for (long i = 0; i < method->GetNumberOfIndexableFields(); ++i)
      read_direct(&method->indexableFields[i], (*this)(it));

   std::memcpy(method->bytecodes, *it, bcLength->GetEmbeddedInteger());
   it += bcLength->GetEmbeddedInteger();   

   return _store_ptr(method);
}

GCString* ObjectDeserializer::createString(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCString*>(obj);
   }

   decltype(strlen("")) string_len;

   read(&string_len, *it);
   it += sizeof(decltype(strlen("")));

   char chars[256] {};

   for(long i = 0; i < string_len; ++i) {
      read(&chars[i], *it);
      it += 1;
   }

   long additionalBytes = header.size - sizeof(VMString);

   VMString* s = new (GetHeap<HEAP_CLS>(), additionalBytes) VMString(chars);
   oldNewAddresses[header] = s;
   
   return _store_ptr(s);
}

GCSymbol* ObjectDeserializer::createSymbol(MetadataIterator& it)
{
   auto header = it.header();

   it += sizeof(ItemHeader);

   if (auto obj = isSeenObject(header); obj) {
      return static_cast<GCSymbol*>(obj);
   }

   VMString* str = load_ptr(createString(it));   

   auto* universe = GetUniverse();
   VMSymbol* s = universe->NewSymbol(str->chars);

   oldNewAddresses[header] = s;

   read(&s->numberOfArgumentsOfSignature, *it);
   it += sizeof(s->numberOfArgumentsOfSignature);

   read(&s->card, *it);
   it += sizeof(s->card);

   return _store_ptr(s);
}
