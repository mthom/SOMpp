#pragma once

#include <algorithm>
#include <vector>

#include "SOMCompositeCache.hpp"
#include "BufferWriter.hpp"

class SOMCacheWriter: public BufferWriter
{
public:
    explicit SOMCacheWriter(SOMCompositeCache* cache)
        : cache(cache)
    {
        // the prelude without compiled methods or relocations is
        // about 95 bytes in size.
        contents.reserve(95 * 1024);
    }

    void writeHeader(SOMCacheMetadataItemHeader header)
    {
        _write<SOMCacheMetadataItemHeader>(header);
    }

    void write(gc_oop_t ptr) {
        _write<gc_oop_t>(ptr);
    }

    void write(uint64_t ptr) {
        _write<uint64_t>(ptr);
    }

    void write(char* ptr, size_t length)
    {
        for(long i = 0; i < length; ++i) {
	   contents.push_back(ptr[i]);
	}
    }

    void write(double ptr) {
        _write<double>(ptr);
    }

    void write(int64_t ptr) {
        _write<int64_t>(ptr);
    }

    void write(bool ptr) {
        _write<bool>(ptr);
    }

    void write(int ptr) {
        _write<int>(ptr);
    }

    void write(uint8_t* ptr) {
        _write<uint8_t*>(ptr);
    }

    void writeBytecodes(uint8_t* bytecodes, size_t length) {
       std::copy(bytecodes, bytecodes + length, std::back_inserter(contents));
    }

    void flushToCache() {
       cache->copyPreludeBuffer(contents.data(), contents.size());
    }
  
private:
    template <typename T>
    void _write(T item) {
       char* itemPtr = reinterpret_cast<char*>(&item);
       std::copy(itemPtr, itemPtr + sizeof(T), std::back_inserter(contents));
    }
    
    SOMCompositeCache *cache;
    std::vector<char> contents;
};
