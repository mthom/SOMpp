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

    ObjectSerializer(std::set<ItemHeader>&& seenAddresses, BufferWriter* writer)
        : seenAddresses(std::move(seenAddresses))
	, bufferWriter(writer)
    {}

    std::set<ItemHeader>&& takeSeenAddresses() {
        return std::move(seenAddresses);
    }
  
    void operator()(VMClass*) override;
    void operator()(VMObject*, long numberOfNonIntrinsicFields = 0) override;
    void operator()(VMArray*) override;
    void operator()(VMBlock*) override;
    void operator()(VMDouble*) override;
    void operator()(VMInteger*) override;
    void operator()(VMInvokable*) override;
    void operator()(VMPrimitive*) override;
    void operator()(VMEvaluationPrimitive*) override;
    void operator()(VMFrame*) override;
    void operator()(VMMethod*) override;
    void operator()(VMString*) override;
    void operator()(VMSymbol*) override;

private:
    void serializeAndWrite(vm_oop_t obj);
    bool checkForSeenAddress(ItemHeader::ItemDesc desc, gc_oop_t obj);

    BufferWriter *bufferWriter;
    std::set<ItemHeader> seenAddresses;
};
