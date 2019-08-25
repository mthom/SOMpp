#pragma once

#include <set>

#include "ObjectFormats.h"
#include "BufferWriter.hpp"
#include "SOMObjectVisitor.hpp"

#include "../misc/defs.h"

class VMClass;
class VMObject;
class VMArray;
class VMBlock;
class VMDouble;
class VMInteger;
class VMInvokable;
class VMPrimitive;
class VMEvaluationPrimitive;
class VMFrame;
class VMMethod;
class VMString;
class VMSymbol;

using ItemHeader = SOMCacheMetadataItemHeader;

class ObjectSerializer: public SOMObjectVisitor {
public:
    ObjectSerializer(BufferWriter* bufferWriter)
        : bufferWriter(bufferWriter)
    {}
  
    void serializeAndWrite(vm_oop_t obj);

    inline bool checkForSeenAddress(ItemHeader::ItemDesc desc, gc_oop_t obj) {
        AbstractVMObject* vm_obj = dynamic_cast<AbstractVMObject*>(load_ptr(obj));

	if (vm_obj == nullptr)
	   return false;

	auto header = ItemHeader { desc, vm_obj, vm_obj->GetObjectSize() };	
        auto it = seenAddresses.find(header);

	if (it != seenAddresses.end()) {
	   bufferWriter->writeHeader(header);
	   return true;
	}

	return false;
    }

    void operator()(VMClass*);
    void operator()(VMObject*, long numberOfNonIntrinsicFields = 0);
    void operator()(VMArray*);
    void operator()(VMBlock*);
    void operator()(VMDouble*);
    void operator()(VMInteger*);
    void operator()(VMInvokable*);
    void operator()(VMPrimitive*);
    void operator()(VMEvaluationPrimitive*);
    void operator()(VMFrame*);
    void operator()(VMMethod*);
    void operator()(VMString*);
    void operator()(VMSymbol*);

private:
    BufferWriter *bufferWriter;
    std::set<ItemHeader> seenAddresses;
};
