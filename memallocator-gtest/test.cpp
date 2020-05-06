#include "pch.h"
#include "gtest/gtest.h"
#include "../memory-allocator/memallocator.c"


TEST(GoToRightNeighbour, GoToRightBlock_ReturnPtrToRightBlock) {
	int size = memgetblocksize() + memgetminimumsize() + 2;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	char* leftBlock = ((char*)ptr) + memgetminimumsize() - memgetblocksize();
	((DESCRIPT*)leftBlock)->size = memgetblocksize() + 1;
	char* rightBlock = (((char*)ptr) + memgetminimumsize() + 1);
	ASSERT_EQ(rightBlock, (char*)(GoToRightNeighbour((DESCRIPT*)leftBlock)));
}


TEST(GoToRightNeighbour, GetOnRightBorder_ReturnNULL) {
	int size = memgetblocksize() + memgetminimumsize() + 2;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	char* rightBlock = ((char*)ptr + memgetminimumsize() + 1);
	((DESCRIPT*)rightBlock)->size = memgetblocksize() + 1;
	ASSERT_EQ(NULL, GoToRightNeighbour((DESCRIPT*)rightBlock));
}


TEST(GoToRightNeighbour, CrossRightBorderOverNBytes_ReturnNULL) {
	const int N = 5;
	int size = memgetblocksize() + memgetminimumsize() + 2;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	char* rightBlock = ((char*)ptr + memgetminimumsize() + 1);
	((DESCRIPT*)rightBlock)->size = memgetblocksize() + 1 + N;
	ASSERT_EQ(NULL, GoToRightNeighbour((DESCRIPT*)rightBlock));
}



TEST(GoToLeftNeighbour, GoToLeftBlock_ReturnPtrToLeftBlock) {
	int size = 2 * memgetblocksize() + memgetminimumsize() + 3;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	int sizeOfBlock = memgetblocksize() + 1;
	char* lastBlock = (char*)ptr + memgetminimumsize() + 1 + sizeOfBlock;
	*((int*)(lastBlock - sizeof(int))) = sizeOfBlock;
	char* midBlock = (char*)ptr + memgetminimumsize() + 1;
	ASSERT_EQ(midBlock, (char*)(GoToLeftNeighbour((DESCRIPT*)lastBlock)));
}


TEST(GoToLeftNeighbour, CrossLeftBorderOverNBytes_ReturnNULL) {
	const int N = 5;
	int size = memgetblocksize() + memgetminimumsize() + 2;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	char* rightBlock = (char*)ptr + memgetminimumsize() + 1;
	*((int*)(rightBlock - sizeof(int))) = memgetblocksize() + 1 + N;
	ASSERT_EQ(NULL, GoToLeftNeighbour((DESCRIPT*)rightBlock));
}


TEST(GoToLeftNeighbour, GetOnLeftBorder_ReturnPtrToLeftBlock) {
	int size = memgetblocksize() + memgetminimumsize() + 2;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	char* rightBlock = (char*)ptr + memgetminimumsize() + 1;
	*((int*)(rightBlock - sizeof(int))) = memgetblocksize() + 1;
	char* leftBlock = (char*)ptr + memgetminimumsize() - memgetblocksize();
	ASSERT_EQ(leftBlock, (char*)(GoToLeftNeighbour((DESCRIPT*)rightBlock)));
}



TEST(SetBlockSize, SetSize_SetSizeInTwoPlaces) {
	DESCRIPT* ptr = (DESCRIPT*)malloc(memgetblocksize() + 1);
	SetBlockSize(ptr, memgetblocksize() + 1);
	int sizeFromRight = *((int*)((char*)ptr + memgetblocksize() + 1 - sizeof(int)));
	ASSERT_EQ(memgetblocksize() + 1, ptr->size);
	ASSERT_EQ(memgetblocksize() + 1, sizeFromRight);
}



TEST(SetBlockSize, ChangeSizeToNewSize_ChangeSizeInTwoPlaces) {
	const int newSize = 8;
	DESCRIPT* ptr = (DESCRIPT*)malloc(memgetblocksize() + 1);
	ptr->size = memgetblocksize();
	*((int*)((char*)ptr + memgetblocksize() + 1 - sizeof(int))) = memgetblocksize() + 1;
	SetBlockSize(ptr, newSize);
	int sizeFromRight = *((int*)((char*)ptr + newSize - sizeof(int)));
	ASSERT_EQ(newSize, ptr->size);
	ASSERT_EQ(newSize, sizeFromRight);
}



TEST(SplitBlock, SplitBlockByHalf_ReturnPtrToSplittedPart) {
	DESCRIPT* block = (DESCRIPT*)malloc(2 * (memgetblocksize() + 1));
	block->size = 2 * (memgetblocksize() + 1);
	*((int*)GoToLastSize(block)) = 2 * (memgetblocksize() + 1);
	DESCRIPT* rightBlock = SplitBlock(block, memgetblocksize() + 1);
	ASSERT_EQ((char*)block + memgetblocksize() + 1, (char*)rightBlock);
	int leftBlockSize = *((int*)((char*)block + memgetblocksize() + 1 - sizeof(int)));
	int rightBlockSize = *((int*)((char*)block + 2 * (memgetblocksize() + 1) - sizeof(int)));
	ASSERT_EQ(memgetblocksize() + 1, block->size);
	ASSERT_EQ(memgetblocksize() + 1, leftBlockSize);
	ASSERT_EQ(memgetblocksize() + 1, rightBlock->size);
	ASSERT_EQ(memgetblocksize() + 1, rightBlockSize);
}


TEST(SplitBlock, SplitBlockOnBiggerNBytes_ReturnNULL) {
	const int N = 5;
	DESCRIPT* block = (DESCRIPT*)malloc(memgetblocksize() + 1);
	block->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(block)) = memgetblocksize() + 1;
	DESCRIPT* rightBlock = SplitBlock(block, memgetblocksize() + 1 + N);
	ASSERT_EQ(NULL, rightBlock);
	int blockSize = *((int*)((char*)block + memgetblocksize() + 1 - sizeof(int)));
	ASSERT_EQ(memgetblocksize() + 1, block->size);
	ASSERT_EQ(memgetblocksize() + 1, blockSize);
}


TEST(SplitBlock, SplitNoSpaceForTwoDescriptors_ReturnNULL) {
	DESCRIPT* block = (DESCRIPT*)malloc(2 * (memgetblocksize() + 1));
	block->size = 2 * (memgetblocksize() + 1);
	*((int*)GoToLastSize(block)) = 2 * (memgetblocksize() + 1);
	DESCRIPT* rightBlock = SplitBlock(block, 2 * memgetblocksize());
	ASSERT_EQ(NULL, rightBlock);
	int blockSize = *((int*)((char*)block + 2 * (memgetblocksize() + 1) - sizeof(int)));
	ASSERT_EQ(2 * (memgetblocksize() + 1), block->size);
	ASSERT_EQ(2 * (memgetblocksize() + 1), blockSize);
}


TEST(SplitBlock, SplitNoMemoryPoolForOtherPart_ReturnNULL) {
	DESCRIPT* block = (DESCRIPT*)malloc(2 * (memgetblocksize() + 1));
	block->size = 2 * (memgetblocksize() + 1);
	*((int*)GoToLastSize(block)) = 2 * (memgetblocksize() + 1);
	DESCRIPT* rightBlock = SplitBlock(block, memgetblocksize() + 2);
	ASSERT_EQ(NULL, rightBlock);
	int blockSize = *((int*)((char*)block + 2 * (memgetblocksize() + 1) - sizeof(int)));
	ASSERT_EQ(2 * (memgetblocksize() + 1), block->size);
	ASSERT_EQ(2 * (memgetblocksize() + 1), blockSize);
}



TEST(MergeWRight, MergeWAvailable_DoMerge) {
	int size = memgetblocksize() + memgetminimumsize() + 2;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	DESCRIPT* leftBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* rightBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	leftBlock->next = rightBlock;
	leftBlock->prev = rightBlock;
	leftBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(leftBlock)) = memgetblocksize() + 1;
	rightBlock->next = leftBlock;
	rightBlock->prev = leftBlock;
	rightBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(rightBlock)) = memgetblocksize() + 1;
	MergeWRight(leftBlock);
	ASSERT_EQ(2 * (memgetblocksize() + 1), leftBlock->size);
	ASSERT_EQ(2 * (memgetblocksize() + 1), *((int*)((char*)ptr + size - sizeof(int))));
	ASSERT_EQ(leftBlock, leftBlock->next);
	ASSERT_EQ(leftBlock, leftBlock->prev);
}


TEST(MergeWRight, MergeWUnavailable_NoMerge) {
	int size = memgetblocksize() + memgetminimumsize() + 2;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	DESCRIPT* leftBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* rightBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	leftBlock->next = leftBlock;
	leftBlock->prev = leftBlock;
	leftBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(leftBlock)) = memgetblocksize() + 1;
	rightBlock->next = NULL;
	rightBlock->prev = NULL;
	rightBlock->size = -(memgetblocksize() + 1);
	*((int*)GoToLastSize(rightBlock)) = -(memgetblocksize() + 1);
	MergeWRight(leftBlock);
	ASSERT_EQ(memgetblocksize() + 1, leftBlock->size);
	ASSERT_EQ(memgetblocksize() + 1, *((int*)((char*)leftBlock + memgetblocksize() + 1 - sizeof(int))));
	ASSERT_EQ(leftBlock, leftBlock->next);
	ASSERT_EQ(leftBlock, leftBlock->prev);

	ASSERT_EQ(-(memgetblocksize() + 1), rightBlock->size);
	ASSERT_EQ(-(memgetblocksize() + 1), *((int*)((char*)rightBlock + memgetblocksize() + 1 - sizeof(int))));
	ASSERT_EQ(NULL, rightBlock->next);
	ASSERT_EQ(NULL, rightBlock->prev);
}



TEST(SearchBlock, SecondBlockWEnoughMemory_ReturnPtrToSecondBlock) {
	int size = memgetblocksize() + memgetminimumsize() + 3;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	DESCRIPT* firstBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* secondBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	firstBlock->next = secondBlock;
	firstBlock->prev = secondBlock;
	firstBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(firstBlock)) = memgetblocksize() + 1;
	secondBlock->next = firstBlock;
	secondBlock->prev = firstBlock;
	secondBlock->size = memgetblocksize() + 2;
	*((int*)GoToLastSize(secondBlock)) = memgetblocksize() + 2;
	ASSERT_EQ(secondBlock, SearchBlock(2));
}


TEST(SearchBlock, FirstBlockWEnoughMemory_ReturnPtrToFirstBlock) {
	int size = memgetblocksize() + memgetminimumsize() + 3;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	DESCRIPT* firstBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* secondBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	firstBlock->next = secondBlock;
	firstBlock->prev = secondBlock;
	firstBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(firstBlock)) = memgetblocksize() + 1;
	secondBlock->next = firstBlock;
	secondBlock->prev = firstBlock;
	secondBlock->size = memgetblocksize() + 2;
	*((int*)GoToLastSize(secondBlock)) = memgetblocksize() + 2;
	ASSERT_EQ(firstBlock, SearchBlock(1));
}


TEST(SearchBlock, NoBlockWithEnoughMemory_ReturnNULL) {
	const int tooBig = 10;
	int size = memgetblocksize() + memgetminimumsize() + 3;
	void* ptr = (void*)malloc(size);
	meminit(ptr, size);
	DESCRIPT* firstBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* secondBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	firstBlock->next = secondBlock;
	firstBlock->prev = secondBlock;
	firstBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(firstBlock)) = memgetblocksize() + 1;
	secondBlock->next = firstBlock;
	secondBlock->prev = firstBlock;
	secondBlock->size = memgetblocksize() + 2;
	*((int*)GoToLastSize(secondBlock)) = memgetblocksize() + 2;
	ASSERT_EQ(NULL, SearchBlock(tooBig));
}



TEST(AddToFreeList, ListWOneNode_ValidList) {
	int size = memgetminimumsize() + 1;
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* originalNode = ((DESCRIPT_G*)ptr)->freeList;
	DESCRIPT* blockToAdd = (DESCRIPT*)malloc(sizeof(DESCRIPT));
	AddToFreeList(blockToAdd);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, blockToAdd);
	ASSERT_EQ(originalNode->next, blockToAdd);
	ASSERT_EQ(originalNode->prev, blockToAdd);
	ASSERT_EQ(originalNode, blockToAdd->next);
	ASSERT_EQ(originalNode, blockToAdd->prev);
}


TEST(AddToFreeList, ListWTwoNodes_ValidList) {
	int size = memgetminimumsize() + 1;
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* firstNode = ((DESCRIPT_G*)ptr)->freeList;
	DESCRIPT* secondNode = (DESCRIPT*)malloc(sizeof(DESCRIPT));
	secondNode->next = secondNode->prev = firstNode;
	firstNode->next = firstNode->prev = secondNode;
	DESCRIPT* blockToAdd = (DESCRIPT*)malloc(sizeof(DESCRIPT));
	AddToFreeList(blockToAdd);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, blockToAdd);
	ASSERT_EQ(secondNode->next, blockToAdd);
	ASSERT_EQ(firstNode->prev, blockToAdd);
	ASSERT_EQ(firstNode, blockToAdd->next);
	ASSERT_EQ(secondNode, blockToAdd->prev);
}


TEST(AddToFreeList, EmptyList_ValidList) {
	int size = memgetminimumsize() + 1;
	void* ptr = malloc(size);
	meminit(ptr, size);
	((DESCRIPT_G*)ptr)->freeList = NULL;
	DESCRIPT* blockToAdd = (DESCRIPT*)malloc(sizeof(DESCRIPT));
	AddToFreeList(blockToAdd);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, blockToAdd);
	ASSERT_EQ(blockToAdd, blockToAdd->next);
	ASSERT_EQ(blockToAdd, blockToAdd->prev);
}



TEST(DelFromFreeList, ListWOneNode_EmptyList) {
	int size = memgetminimumsize() + 1;
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* originalNode = ((DESCRIPT_G*)ptr)->freeList;
	DelFromFreeList(originalNode);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
}


TEST(DelFromFreeList, ListWTwoNodesDelFromHead_ValidList) {
	int size = memgetminimumsize() + 1;
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* firstNode = ((DESCRIPT_G*)ptr)->freeList;
	DESCRIPT* secondNode = (DESCRIPT*)malloc(sizeof(DESCRIPT));
	secondNode->next = secondNode->prev = firstNode;
	firstNode->next = firstNode->prev = secondNode;
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	DelFromFreeList(freeListHead);
	freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, secondNode);
	ASSERT_EQ(secondNode->next, secondNode);
	ASSERT_EQ(secondNode->prev, secondNode);
}


TEST(DelFromFreeList, ListWTwoNodesDelNotFromHead_ValidList) {
	int size = memgetminimumsize() + 1;
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* firstNode = ((DESCRIPT_G*)ptr)->freeList;
	DESCRIPT* secondNode = (DESCRIPT*)malloc(sizeof(DESCRIPT));
	secondNode->next = secondNode->prev = firstNode;
	firstNode->next = firstNode->prev = secondNode;
	DelFromFreeList(secondNode);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, firstNode);
	ASSERT_EQ(firstNode->next, firstNode);
	ASSERT_EQ(firstNode->prev, firstNode);
}


TEST(DelFromFreeList, ListWThreeNodesDelFromMid_ValidList) {
	int size = memgetminimumsize() + 1;
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* firstNode = ((DESCRIPT_G*)ptr)->freeList;
	DESCRIPT* secondNode = (DESCRIPT*)malloc(sizeof(DESCRIPT));
	DESCRIPT* thirdNode = (DESCRIPT*)malloc(sizeof(DESCRIPT));
	firstNode->next = thirdNode->prev = secondNode;
	secondNode->next = firstNode->prev = thirdNode;
	thirdNode->next = secondNode->prev = firstNode;
	DelFromFreeList(secondNode);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, firstNode);
	ASSERT_EQ(firstNode->next, thirdNode);
	ASSERT_EQ(firstNode->prev, thirdNode);
	ASSERT_EQ(thirdNode->next, firstNode);
	ASSERT_EQ(thirdNode->prev, firstNode);
}



TEST(memalloc, FreeListWNodeBigMemPool_SplitBlock) {
	int size = memgetminimumsize() + 1 + 2 * (memgetblocksize() + 1);
	void* ptr = malloc(size);
	meminit(ptr, size);
	void* mem = memalloc(1);
	DESCRIPT* freeBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* allocatedBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1 + memgetblocksize() + 1);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, freeBlock);
	ASSERT_EQ(2 * (memgetblocksize() + 1), freeBlock->size);
	ASSERT_EQ(2 * (memgetblocksize() + 1), *((int*)GoToLastSize(freeBlock)));
	ASSERT_EQ(freeBlock, freeBlock->next);
	ASSERT_EQ(freeBlock, freeBlock->prev);

	ASSERT_EQ(-(memgetblocksize() + 1), allocatedBlock->size);
	ASSERT_EQ(-(memgetblocksize() + 1), *((int*)GoToLastSize(allocatedBlock)));
}


TEST(memalloc, FreeListWNodeFitMemPool_TakeWholeBlock) {
	int size = memgetminimumsize() + 2;
	void* ptr = malloc(size);
	meminit(ptr, size);
	void* mem = memalloc(1);
	DESCRIPT* allocatedBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(NULL, freeListHead);
	ASSERT_EQ(-(memgetblocksize() + 2), allocatedBlock->size);
	ASSERT_EQ(-(memgetblocksize() + 2), *((int*)GoToLastSize(allocatedBlock)));
}



TEST(memfree, EmptyFreeList_ValidList) {
	int size = memgetminimumsize() + 1;
	void* ptr = malloc(size);
	meminit(ptr, size);
	((DESCRIPT_G*)ptr)->freeList = NULL;
	DESCRIPT* allocatedBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	allocatedBlock->size *= -1;
	*((int*)GoToLastSize(allocatedBlock)) *= -1;
	allocatedBlock->next = allocatedBlock->prev = NULL;
	void* memoryToFree = (void*)(GetMemory(allocatedBlock));
	memfree(memoryToFree);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, allocatedBlock);
	ASSERT_EQ(memgetblocksize() + 1, allocatedBlock->size);
	ASSERT_EQ(memgetblocksize() + 1, *((int*)GoToLastSize(allocatedBlock)));
	ASSERT_EQ(allocatedBlock, allocatedBlock->next);
	ASSERT_EQ(allocatedBlock, allocatedBlock->prev);
}


TEST(memfree, BlockFromRightIsFree_ValidListWOneNode) {
	int size = memgetminimumsize() + 2 + memgetblocksize();
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* allocatedBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* freeBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	allocatedBlock->size = -(memgetblocksize() + 1);
	*((int*)GoToLastSize(allocatedBlock)) = -(memgetblocksize() + 1);
	allocatedBlock->next = allocatedBlock->prev = NULL;
	freeBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(freeBlock)) = memgetblocksize() + 1;
	freeBlock->next = freeBlock->prev = freeBlock;
	((DESCRIPT_G*)ptr)->freeList = freeBlock;
	void* memoryToFree = (void*)(GetMemory(allocatedBlock));
	memfree(memoryToFree);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, allocatedBlock);
	ASSERT_EQ(2 * (memgetblocksize() + 1), allocatedBlock->size);
	ASSERT_EQ(2 * (memgetblocksize() + 1), *((int*)GoToLastSize(allocatedBlock)));
	ASSERT_EQ(allocatedBlock, allocatedBlock->next);
	ASSERT_EQ(allocatedBlock, allocatedBlock->prev);
}


TEST(memfree, BlockFromLeftIsFree_ValidListWOneNode) {
	int size = memgetminimumsize() + 2 + memgetblocksize();
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* freeBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* allocatedBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	allocatedBlock->size = -(memgetblocksize() + 1);
	*((int*)GoToLastSize(allocatedBlock)) = -(memgetblocksize() + 1);
	allocatedBlock->next = allocatedBlock->prev = NULL;
	freeBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(freeBlock)) = memgetblocksize() + 1;
	freeBlock->next = freeBlock->prev = freeBlock;
	((DESCRIPT_G*)ptr)->freeList = freeBlock;
	void* memoryToFree = (void*)(GetMemory(allocatedBlock));
	memfree(memoryToFree);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, freeBlock);
	ASSERT_EQ(2 * (memgetblocksize() + 1), freeBlock->size);
	ASSERT_EQ(2 * (memgetblocksize() + 1), *((int*)GoToLastSize(freeBlock)));
	ASSERT_EQ(freeBlock, freeBlock->next);
	ASSERT_EQ(freeBlock, freeBlock->prev);
}


TEST(memfree, BlocksFromLeftNRightAreFree_ValidListWOneNode) {
	int size = memgetminimumsize() + 1 + 2 * (memgetblocksize() + 1);
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* leftFreeBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* midAllocatedBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	DESCRIPT* rightFreeBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1 + memgetblocksize() + 1);
	leftFreeBlock->size = rightFreeBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(leftFreeBlock)) = *((int*)GoToLastSize(rightFreeBlock)) = memgetblocksize() + 1;
	leftFreeBlock->next = leftFreeBlock->prev = rightFreeBlock;
	rightFreeBlock->next = rightFreeBlock->prev = leftFreeBlock;
	midAllocatedBlock->size = -(memgetblocksize() + 1);
	*((int*)GoToLastSize(midAllocatedBlock)) = -(memgetblocksize() + 1);
	midAllocatedBlock->next = midAllocatedBlock->prev = NULL;
	((DESCRIPT_G*)ptr)->freeList = rightFreeBlock;
	void* memoryToFree = (void*)(GetMemory(midAllocatedBlock));
	memfree(memoryToFree);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, leftFreeBlock);
	ASSERT_EQ(3 * (memgetblocksize() + 1), leftFreeBlock->size);
	ASSERT_EQ(3 * (memgetblocksize() + 1), *((int*)GoToLastSize(leftFreeBlock)));
	ASSERT_EQ(leftFreeBlock, leftFreeBlock->next);
	ASSERT_EQ(leftFreeBlock, leftFreeBlock->prev);
}


TEST(memfree, BlockInMidIsOccFreeLeftBlock_ValidListWTwoNodes) {
	int size = memgetminimumsize() + 1 + 2 * (memgetblocksize() + 1);
	void* ptr = malloc(size);
	meminit(ptr, size);
	DESCRIPT* leftAllocatedBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() - memgetblocksize());
	DESCRIPT* midOccBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1);
	DESCRIPT* rightFreeBlock = (DESCRIPT*)((char*)ptr + memgetminimumsize() + 1 + memgetblocksize() + 1);
	rightFreeBlock->size = memgetblocksize() + 1;
	*((int*)GoToLastSize(rightFreeBlock)) = memgetblocksize() + 1;
	rightFreeBlock->prev = rightFreeBlock->next = rightFreeBlock;
	leftAllocatedBlock->size = midOccBlock->size = -(memgetblocksize() + 1);
	*((int*)GoToLastSize(leftAllocatedBlock)) = *((int*)GoToLastSize(midOccBlock)) = -(memgetblocksize() + 1);
	leftAllocatedBlock->next = leftAllocatedBlock->prev = midOccBlock->next = midOccBlock->prev = NULL;
	((DESCRIPT_G*)ptr)->freeList = rightFreeBlock;
	void* memoryToFree = (void*)(GetMemory(leftAllocatedBlock));
	memfree(memoryToFree);
	DESCRIPT* freeListHead = ((DESCRIPT_G*)ptr)->freeList;
	ASSERT_EQ(freeListHead, leftAllocatedBlock);
	ASSERT_EQ(rightFreeBlock, leftAllocatedBlock->next);
	ASSERT_EQ(rightFreeBlock, leftAllocatedBlock->prev);
	ASSERT_EQ(leftAllocatedBlock, rightFreeBlock->next);
	ASSERT_EQ(leftAllocatedBlock, rightFreeBlock->prev);
	ASSERT_EQ(memgetblocksize() + 1, leftAllocatedBlock->size);
	ASSERT_EQ(memgetblocksize() + 1, *((int*)GoToLastSize(leftAllocatedBlock)));
	ASSERT_EQ(memgetblocksize() + 1, rightFreeBlock->size);
	ASSERT_EQ(memgetblocksize() + 1, *((int*)GoToLastSize(rightFreeBlock)));
}