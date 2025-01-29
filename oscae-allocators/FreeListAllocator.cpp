#include "FreeListAllocator.h"
#include <windows.h>

FreeListAllocator::FreeListAllocator(size_t size) {
	freeList = reinterpret_cast<FreeBlock*>(VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	freeList->size = size;
	freeList->next = nullptr;

	totalSize = size;
}

FreeListAllocator::~FreeListAllocator() {
    VirtualFree(freeList, totalSize, MEM_RELEASE);
}

void* FreeListAllocator::alloc(size_t size, size_t alignment) {
	FreeBlock* prev = nullptr;
	FreeBlock* current = freeList;
	// find first block that is large enough
	while (current != nullptr) {
		size_t offset = (alignment - ((reinterpret_cast<uintptr_t>(current) + sizeof(FreeBlock)) % alignment)) % alignment;
		uintptr_t ptr = reinterpret_cast<uintptr_t>(current) + sizeof(FreeBlock) + offset;
		if (current->size - sizeof(FreeBlock) < size + offset) {
			// not enough space in this block, go next
			prev = current;
			current = current->next;
		}
		else {
			// found a block that is large enough
			if (current->size - sizeof(FreeBlock) - size - offset <= sizeof(FreeBlock)) { // not worth to split
				if (prev == nullptr)
					freeList = current->next;
				else
					prev->next = current->next;

				// create block header
				size_t blockSize = current->size;
				FreeBlock* blockStart = current; // pointer to start of the block

				FreeBlock* blockHeader = reinterpret_cast<FreeBlock*>(ptr - sizeof(FreeBlock)); // get the block header adress
				blockHeader->size = blockSize;
				blockHeader->next = blockStart; // point to the start of the block without offset

				// return the pointer to the start of the block
				return reinterpret_cast<void*>(ptr);
			}
			else {
				// split the block
				FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<uintptr_t>(current) + sizeof(FreeBlock) + offset + size);
				newBlock->size = current->size - sizeof(FreeBlock) - offset - size;
				newBlock->next = current->next;

				if (prev == nullptr)
					freeList = newBlock;
				else
					prev->next = newBlock;

				// create header for the allocated space
				size_t blockSize = sizeof(FreeBlock) + offset + size;
				FreeBlock* blockStart = current; // pointer to start of the block

				FreeBlock* blockHeader = reinterpret_cast<FreeBlock*>(ptr - sizeof(FreeBlock)); // get the block header adress
				blockHeader->size = blockSize;
				blockHeader->next = blockStart; // point to the start of the block without offset

				// return the pointer to the start of the block
				return reinterpret_cast<void*>(ptr);
			}
		}
	}

	return nullptr; // no block large enough
}

void FreeListAllocator::free(void* ptr) {
	// insert block back into the list at an ordered position
	FreeBlock* blockHeader = reinterpret_cast<FreeBlock*>(reinterpret_cast<uintptr_t>(ptr) - sizeof(FreeBlock));
	FreeBlock* newBlock = blockHeader->next;

	// get block header data
	size_t blockSize = blockHeader->size;

	// set its size
	newBlock->size = blockSize;

	// insert into the list
	//edge case when the block is the first one
	if (reinterpret_cast<uintptr_t>(freeList) > reinterpret_cast<uintptr_t>(newBlock)) {
		// insert first in freeList
		FreeBlock* next = freeList;
		freeList = newBlock;
		newBlock->next = next;
	}
	else {

		FreeBlock* current = freeList;

		while (current != nullptr)
		{
			if (current->next == nullptr) {
				// insert at the end
				current->next = newBlock;
				newBlock->next = nullptr;
				break;
			}
			else if (reinterpret_cast<uintptr_t>(current->next) > reinterpret_cast<uintptr_t>(newBlock)) {
				// insert after current
				FreeBlock* next = current->next;
				current->next = newBlock;
				newBlock->next = next;
				break;
			}
			current = current->next;
		}

	}
	// combine adjacent blocks
	FreeBlock* current = freeList;
	while (current->next != nullptr) {
		if (reinterpret_cast<uintptr_t>(current) + current->size == reinterpret_cast<uintptr_t>(current->next)) {
			// combine
			current->size += current->next->size;
			current->next = current->next->next;
		}
		else {
			current = current->next;
		}
	}
}

//void* FreeListAllocator::resize(void* ptr, size_t new_size) {
//	// get block header
//  // check if there is enough space to resize
//  // if not, allocate new block, copy data, free old block
//  // how do we know the alignment of the old block?
//}