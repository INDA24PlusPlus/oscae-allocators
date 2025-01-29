#pragma once
#include <cstddef>

class FreeListAllocator
{
public:
    FreeListAllocator(size_t size);
    ~FreeListAllocator();

    void* alloc(size_t size, size_t alignment);
    void free(void* ptr);
    //void* resize(void* ptr, size_t new_size);

private:
    // Start of a free memory block
    // Also the start of a used memory block but then next is pointing to the start of the allocated space before the alignment offset
    struct FreeBlock {
        size_t size; // including header
        FreeBlock* next;
    };

    FreeBlock* freeList;
    size_t totalSize;
};

