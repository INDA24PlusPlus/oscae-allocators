#pragma once
#include <cstddef>

class LinearAllocator {
public:
    LinearAllocator(size_t size);
    ~LinearAllocator();

    void* alloc(size_t size, size_t alignment);
    void freeAll();

private:
    void* start;
    void* current;
    size_t totalSize;
};