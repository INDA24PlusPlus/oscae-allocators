#include <iostream>
#include "LinearAllocator.h"
#include "FreeListAllocator.h"

void assert(bool condition, const char message[]) {
	if (!condition)
	{
		std::cout << "Assertion failed: " << message << std::endl;
	}
}

void testFreeListAllocator()
{
	FreeListAllocator allocator(1024);
	allocator.alloc(128, 16);
	void* ptr1 = allocator.alloc(128, 16);
    assert(ptr1 != nullptr, "ptr1 null");
	assert(reinterpret_cast<uintptr_t>(ptr1) % 16 == 0, "ptr1 not aligned");
	
	void* ptr2 = allocator.alloc(79, 13);
	assert(ptr2 != nullptr, "ptr2 null");
	assert(reinterpret_cast<uintptr_t>(ptr2) % 13 == 0, "ptr2 not aligned");

	void* ptr3 = allocator.alloc(512, 16);
	assert(ptr3 != nullptr, "ptr3 null");
	assert(reinterpret_cast<uintptr_t>(ptr3) % 16 == 0, "ptr3 not aligned");

	void* ptr4 = allocator.alloc(512, 16);
	assert(ptr4 == nullptr, "ptr4 not null");

	allocator.free(ptr3);

	void* ptr5 = allocator.alloc(512, 16);
	assert(ptr5 != nullptr, "ptr5 null, might be that combining is not working");
	assert(reinterpret_cast<uintptr_t>(ptr5) % 16 == 0, "ptr5 not aligned");
}

int main()
{
	testFreeListAllocator();
}