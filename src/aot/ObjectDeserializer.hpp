#pragma once

#include <map>
#include <memory>
#include <vector>

#include "../vmobjects/ObjectFormats.h"
#include "BufferWriter.hpp"
#include "../../omr/runtime/shared_common/modifications/som_cache/SOMCompositeCache.hpp"

#include "../misc/defs.h"

using MetadataIterator = SOMCacheMetadataEntryIterator;
using ItemHeader = SOMCacheMetadataItemHeader;

class ObjectDeserializer {
public:
    ObjectDeserializer();
    ObjectDeserializer(std::shared_ptr<std::map<AbstractVMObject*, AbstractVMObject*>>&);

    virtual ~ObjectDeserializer();

    gc_oop_t operator()(MetadataIterator&);

    uint64_t getMaxCard() { return card; }

    std::map<ItemHeader, AbstractVMObject*>* GetAddressOfOldNewAddresses() {
        return &oldNewAddresses;
    }

    std::map<ItemHeader, AbstractVMObject*>& GetOldNewAddresses() {
        return oldNewAddresses;
    }

    const std::vector<VMClass*>& GetPreprocessedClasses() const {
        return classes;
    }

    auto GetReverseLookupMap() {
        return reverseLookupMap;
    }

    void resetAddressMap() {
        oldNewAddresses.clear();
    }

private:
    gc_oop_t isSeenObject(const ItemHeader&);
    void eraseAllSubObjectsAtAddress(AbstractVMObject*);

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
    std::shared_ptr<std::map<AbstractVMObject*, AbstractVMObject*>> reverseLookupMap; // reverse lookup for relocations.
    std::vector<VMClass*> classes;

    uint64_t card = 0;
};
