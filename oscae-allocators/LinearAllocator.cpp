#include "LinearAllocator.h"
#include <windows.h>

LinearAllocator::LinearAllocator(size_t size) {
    start = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    current = start;
    totalSize = size;
}

LinearAllocator::~LinearAllocator() {
    VirtualFree(start, 0, MEM_RELEASE);
}

void* LinearAllocator::alloc(size_t size, size_t alignment) {
    uintptr_t currentAddress = reinterpret_cast<uintptr_t>(current);
    uintptr_t offset = (alignment - (currentAddress % alignment)) % alignment;
    uintptr_t alignedAddress = currentAddress + offset;

    current = reinterpret_cast<void*>(alignedAddress + size);
    return reinterpret_cast<void*>(alignedAddress);
}

void LinearAllocator::freeAll() {
	current = start;
}