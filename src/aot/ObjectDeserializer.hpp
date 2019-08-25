#pragma once

#include <map>
#include <vector>

#include "ObjectFormats.h"
#include "BufferWriter.hpp"
#include "SOMCompositeCache.hpp"

#include "../misc/defs.h"

using MetadataIterator = SOMCacheMetadataEntryIterator;
using ItemHeader = SOMCacheMetadataItemHeader;

class ObjectDeserializer {
public:
    gc_oop_t operator()(MetadataIterator&);  

private:
    gc_oop_t isSeenObject(const ItemHeader&);

    GCObject* createObject(MetadataIterator&, long numberOfIntrinsicFields = 0);
    GCArray* createArray(MetadataIterator&);
    GCBlock* createBlock(MetadataIterator&);
    GCClass* createClass(MetadataIterator&);
    GCInteger* createInteger(MetadataIterator&);
    GCDouble* createDouble(MetadataIterator&);
    GCEvaluationPrimitive* createEvaluationPrimitive(MetadataIterator&);
  //    gc_oop_t createFrame(MetadataIterator&);
    GCMethod* createMethod(MetadataIterator&);
    GCPrimitive* createPrimitive(MetadataIterator&);
    GCString* createString(MetadataIterator&);
    GCSymbol* createSymbol(MetadataIterator&);

private:
    std::map<ItemHeader, AbstractVMObject*> oldNewAddresses;
};
