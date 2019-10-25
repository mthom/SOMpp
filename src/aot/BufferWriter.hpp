#pragma once

#include "../omr/runtime/shared_common/modifications/som_cache/SOMMetadataSectionEntryIterator.hpp"

struct SOMCacheMetadataItemHeader;

class BufferWriter {
public:
    virtual ~BufferWriter() {}

    virtual void writeHeader(SOMCacheMetadataItemHeader) = 0;

    virtual void write(gc_oop_t) = 0;
    virtual void write(uint64_t) = 0;
    virtual void write(char*, size_t) = 0;
    virtual void write(double) = 0;
    virtual void write(int64_t) = 0;
    virtual void write(bool) = 0;
    virtual void write(int) = 0;
    virtual void writeBytecodes(uint8_t*, size_t) = 0;
};
