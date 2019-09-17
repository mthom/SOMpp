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

#include "../omr/compiler/env/CompilerEnv.hpp"

#include "ObjectSerializer.hpp"

void ObjectSerializer::serializeAndWrite(vm_oop_t obj)
{
   if (obj == nullptr)
      return;

   obj->visit(*this);
}

bool ObjectSerializer::checkForSeenAddress(ItemHeader::ItemDesc desc, gc_oop_t obj)
{ 
   AbstractVMObject* vm_obj = dynamic_cast<AbstractVMObject*>(load_ptr(obj));

   if (vm_obj == nullptr)
      return false;
   
   auto* rr_vm_obj = TR::Compiler->aotAdapter.reverseLookup(vm_obj);
   auto header = ItemHeader { desc, rr_vm_obj, vm_obj->GetObjectSize() };	
   auto it = seenAddresses.find(header);

   if (it != seenAddresses.end()) {
      bufferWriter->writeHeader(header);
      return true;
   }

   return false;
}

void ObjectSerializer::operator()(VMClass* clazz)
{
   if (checkForSeenAddress(ItemHeader::clazz, _store_ptr(clazz)))
      return;

   ItemHeader header{ItemHeader::clazz,
       static_cast<AbstractVMObject*>(clazz),
       clazz->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   (*this)(static_cast<VMObject*>(clazz), VMClass::VMClassNumberOfFields);

   serializeAndWrite(load_ptr(clazz->superClass));
   serializeAndWrite(load_ptr(clazz->name));

   // these are both arrays.
   serializeAndWrite(clazz->GetInstanceFields());
   serializeAndWrite(clazz->GetInstanceInvokables());
}

void ObjectSerializer::operator()(VMObject* obj, long numberOfNonIntrinsicFields)
{
   if (checkForSeenAddress(ItemHeader::object, _store_ptr(obj)))
      return;

   ItemHeader header{ItemHeader::object,
       obj,
       obj->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   bufferWriter->write(obj->hash);
   bufferWriter->write(obj->objectSize);
   bufferWriter->write(obj->numberOfFields);
   bufferWriter->write(obj->fieldsOffset);

   serializeAndWrite(load_ptr(obj->clazz));

   for (long i = numberOfNonIntrinsicFields; i < obj->GetNumberOfFields(); ++i) {
      serializeAndWrite(obj->GetField(i));
   }
}

void ObjectSerializer::operator()(VMArray* array)
{
   if (checkForSeenAddress(ItemHeader::array, _store_ptr(array)))
      return;

   ItemHeader header{ItemHeader::array,
       array,
       array->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   (*this)(static_cast<VMObject*>(array), VMArray::VMArrayNumberOfFields);

   for (long i = 0; i < array->GetNumberOfIndexableFields(); ++i)
      serializeAndWrite(array->GetIndexableField(i));
}

void ObjectSerializer::operator()(VMBlock* block)
{
   if (checkForSeenAddress(ItemHeader::block, _store_ptr(block)))
      return;

   ItemHeader header{ItemHeader::block,
       block,
       block->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   (*this)(static_cast<VMObject*>(block), VMBlock::VMBlockNumberOfFields);

   serializeAndWrite(load_ptr(block->blockMethod));
}

void ObjectSerializer::operator()(VMDouble* dbl)
{
   if (checkForSeenAddress(ItemHeader::num_double, _store_ptr(dbl)))
      return;

   ItemHeader header{ItemHeader::num_double,
       dbl,
       dbl->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   bufferWriter->write(dbl->embeddedDouble);
}

void ObjectSerializer::operator()(VMInteger* integer)
{
   if (checkForSeenAddress(ItemHeader::num_integer, _store_ptr(integer)))
      return;

   ItemHeader header{ItemHeader::num_integer,
       integer,
       integer->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   bufferWriter->write(integer->embeddedInteger);
}

void ObjectSerializer::operator()(VMInvokable* invokable)
{
  // VMInvokable is an abstract class, so none of this is needed.
  //        if (checkForSeenAddress(block)) return;
  //	ItemHeader header{ItemHeader::block, block, block->GetObjectSize()};
  //
  // 	bufferWriter->writeHeader(header);
  //
  //	(*this)(static_cast<VMObject*>(block));

   serializeAndWrite(load_ptr(invokable->signature));
   serializeAndWrite(load_ptr(invokable->holder));
}

void ObjectSerializer::operator()(VMPrimitive* primitive)
{
   if (checkForSeenAddress(ItemHeader::prim, _store_ptr(primitive)))
      return;

   ItemHeader header{ItemHeader::prim,
       primitive,
       primitive->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   (*this)(static_cast<VMObject*>(primitive), VMPrimitive::VMPrimitiveNumberOfFields + VMInvokable::VMInvokableNumberOfFields);

   serializeAndWrite(load_ptr(primitive->signature));
   serializeAndWrite(load_ptr(primitive->holder));

   //   bufferWriter->write((PrimitiveRoutine*) nullptr);
   bufferWriter->write(primitive->empty);
}

void ObjectSerializer::operator()(VMEvaluationPrimitive* eval_prim)
{
   if (checkForSeenAddress(ItemHeader::eval_prim, _store_ptr(eval_prim)))
      return;

   ItemHeader header{ItemHeader::eval_prim,
       eval_prim,
       eval_prim->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   (*this)(static_cast<VMObject*>(eval_prim), VMEvaluationPrimitive::VMEvaluationPrimitiveNumberOfFields
	   + VMPrimitive::VMPrimitiveNumberOfFields + VMInvokable::VMInvokableNumberOfFields);

   serializeAndWrite(load_ptr(eval_prim->signature));

   bufferWriter->write(eval_prim->GetNumberOfArguments());

   serializeAndWrite(load_ptr(eval_prim->holder));

   bufferWriter->write(eval_prim->empty);
}

void ObjectSerializer::operator()(VMFrame* frame)
{
   if (checkForSeenAddress(ItemHeader::frame, _store_ptr(frame)))
      return;

   ItemHeader header{ItemHeader::frame,
       frame,
       frame->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   (*this)(static_cast<VMObject*>(frame), VMFrame::VMFrameNumberOfFields);

   serializeAndWrite(load_ptr(frame->previousFrame));
   serializeAndWrite(load_ptr(frame->context));
   serializeAndWrite(load_ptr(frame->method));

#if GC_TYPE == OMR_GARBAGE_COLLECTION
   bufferWriter->write(frame->isJITFrame);
   bufferWriter->write(frame->isJITAllocatedFrame);
   bufferWriter->write(frame->recursiveLevel);
#endif

   bufferWriter->write(frame->bytecodeIndex);

   long i = 0;
   for (auto arg = frame->arguments; arg + i <= frame->stack_ptr; ++i)
      serializeAndWrite(load_ptr(frame->arguments[i]));

   for (i = 0; i < frame->GetMethod()->GetNumberOfLocals(); i++)
      serializeAndWrite(load_ptr(frame->locals[i]));

   //bufferWriter->write((gc_oop_t*) nullptr); // for the stack_ptr.
}

void ObjectSerializer::operator()(VMMethod* method)
{
   if (checkForSeenAddress(ItemHeader::method, _store_ptr(method)))
      return;

   ItemHeader header{ItemHeader::method,
       method,
       method->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   (*this)(static_cast<VMObject*>(method), VMMethod::VMMethodNumberOfFields + VMInvokable::VMInvokableNumberOfFields);

   bufferWriter->write(method->numberOfConstantsEmbedded);

   serializeAndWrite(load_ptr(method->bcLength));

   serializeAndWrite(load_ptr(method->numberOfLocals));
   serializeAndWrite(load_ptr(method->maximumNumberOfStackElements));
   serializeAndWrite(load_ptr(method->numberOfArguments));
   serializeAndWrite(load_ptr(method->numberOfConstants));

   serializeAndWrite(load_ptr(method->signature));
   serializeAndWrite(load_ptr(method->holder));

   for (long i = 0; i < method->GetNumberOfIndexableFields(); ++i)
     serializeAndWrite(method->GetIndexableField(i));

   bufferWriter->writeBytecodes(method->bytecodes, method->GetNumberOfBytecodes());
}

void ObjectSerializer::operator()(VMString* vm_string) {
   if (checkForSeenAddress(ItemHeader::vm_string, _store_ptr(vm_string)))
      return;

   ItemHeader header{ItemHeader::vm_string,
       vm_string,
       vm_string->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   size_t length = strlen(vm_string->chars);
   
   bufferWriter->write(length);
   bufferWriter->write(vm_string->chars, length);
}

void ObjectSerializer::operator()(VMSymbol* symbol)
{
   if (checkForSeenAddress(ItemHeader::symbol, _store_ptr(symbol)))
      return;

   ItemHeader header{ItemHeader::symbol,
       symbol,
       symbol->GetObjectSize()};

   seenAddresses.insert(header);

   bufferWriter->writeHeader(header);

   (*this)(static_cast<VMString*>(symbol));

   bufferWriter->write(symbol->numberOfArgumentsOfSignature);
   /*
   serializeAndWrite(load_ptr(symbol->cachedClass_invokable[0]));
   serializeAndWrite(load_ptr(symbol->cachedClass_invokable[1]));
   serializeAndWrite(load_ptr(symbol->cachedClass_invokable[2]));

   bufferWriter->write(symbol->nextCachePos);

   serializeAndWrite(load_ptr(symbol->cachedInvokable[0]));
   serializeAndWrite(load_ptr(symbol->cachedInvokable[1]));
   serializeAndWrite(load_ptr(symbol->cachedInvokable[2]));
   */
   bufferWriter->write(symbol->card);
}

