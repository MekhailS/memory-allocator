#include "fullcomplex-test.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "memallocator.h"


typedef struct TEST_BLOCK_T {
	void* ptr;	// ptr to allocated memory
	int size;	// size of allocated memory
	int isAlloc; // bool flag
}TEST_BLOCK;


typedef struct TEST_STACK_T {
	TEST_BLOCK tests[TEST_N];	//array on which stack is implemented
	int last_i;	//index of stack's top element
}TEST_STACK;


// swap i-th and j-th elements in test stack
void SwapElem(int i, int j, TEST_BLOCK tests[]) {
	TEST_BLOCK temp = tests[i];
	tests[i] = tests[j];
	tests[j] = temp;
}


// randomly mix elements in test stack
void RandomSwapping(TEST_BLOCK tests[]) {
	for (int i = 0; i < TEST_N; i++) {
		int rand_i = i + rand() % (TEST_N - i);
		SwapElem(i, rand_i, tests);
	}
}


// init empty test stack
void TestStackInit(TEST_STACK* stack) {
	stack->last_i = 0;
}


// push at top of test tack new block 'toAdd'
void TestStackPush(TEST_STACK* stack, TEST_BLOCK toAdd) {
	if (stack->last_i < TEST_N) {
		stack->tests[stack->last_i] = toAdd;
		stack->last_i++;
	}
}


// pop element with index i from test stack
TEST_BLOCK TestStackPop(TEST_STACK* stack, int i) {
	TEST_BLOCK toReturn = stack->tests[i];
	if ((i < stack->last_i) && (stack->last_i > 0)) {
		stack->last_i--;
		// move top element to i-th position in test stack
		stack->tests[i] = stack->tests[stack->last_i];
		return(toReturn);
	}
	return(toReturn);
}


// test stack top() 
int TestStackGetLast(TEST_STACK stack) {
	return(stack.last_i);
}


// pick random emelent from test stack
int TestStackGetRandom(TEST_STACK stack) {
	int toReturn = 0;
	if (stack.last_i > 0) {
		toReturn = rand() % stack.last_i;
	}
	return(toReturn);
}


// check error of writing over the pool allocated for allocator, since the
// memory space for allocator is located in the center of memory pool of
// of size 'MEMORY_N * sizeToAlloc', left and right memory pools of
// of size 'MEMORY_N/2 * sizeToAlloc' are filled with '\0'
int CheckOverWrite(void* ptr, int sizeToAlloc) {
	for (int i = 0; i < MEMORY_N * sizeToAlloc; i++) {
		if (!((i >= (MEMORY_N / 2) * sizeToAlloc) && (i < (MEMORY_N / 2 + 1) * sizeToAlloc))) {
			char* ptrCheck = ((char*)ptr + i);
			char charToCheck = *ptrCheck;
			if (charToCheck != '\0')
				return(1);
		}
	}
	return(0);
}


// free all memory previously allocated by memalloc using memfree
void FreeAllStack(TEST_STACK* stack) {
	for (int i = 0; i < (stack->last_i); i++) {
		memfree(stack->tests[i].ptr);
	}
}


// function perfoms integration test of allocator on random input
// randomly allocates and frees the memory blocks of avg size AVG_MEMBLOCK_SIZE
// 3*TEST_N times, after that frees all allocated memory by memfree()
// iterates this TEST_TRY_N times
// checks override, errors in memfree() and memalloc()
// 0 if no error occured, 1 — otherwise
int FullComplexTest(char *fileName) {
	srand(time(NULL));

	// memory size enough for allocator to allocate TEST_N memory pools of size AVG_MEMBLOCK_SIZE
	int sizeToAlloc = memgetminimumsize() + AVG_MEMBLOCK_SIZE + (TEST_N - 1) * (memgetblocksize() + AVG_MEMBLOCK_SIZE);
	long long int counter = TEST_TRY_N;
	while (counter--) {
		TEST_STACK testStack;
		InitFile(fileName);
		TestStackInit(&testStack);
		void* ptr = malloc(MEMORY_N * sizeToAlloc);
		memset(ptr, '\0', MEMORY_N * sizeToAlloc);

		// memory space for allocator is located in the center of memory pool of
		// of size 'MEMORY_N * sizeToAlloc', left and right memory pools of
		// of size 'MEMORY_N/2 * sizeToAlloc' are filled with '\0'
		// it is done to check error of writing over the pool allocated for allocator
		void* centre = (void*)((char*)ptr + (MEMORY_N / 2) * sizeToAlloc);
		meminit((void*)centre, sizeToAlloc);

		// total memory avaivable for allocator without memory of global descriptor 
		int sizeBeforeAllocation = sizeToAlloc - memgetminimumsize() + memgetblocksize();

		long long int numOfOperations = 3 * TEST_N;
		while (numOfOperations--) {
			int AllocOrFree = rand() % 2;
			switch (AllocOrFree) {
			case 0: // memalloc memory pool of rand size in range 0...2*AVG_MEMBLOCK_SIZE 
			{
				if (TestStackGetLast(testStack) < TEST_N) {
					TEST_BLOCK allocatedBlock;
					allocatedBlock.size = rand() % (2 * AVG_MEMBLOCK_SIZE);
					allocatedBlock.ptr = memalloc(allocatedBlock.size);
					TestStackPush(&testStack, allocatedBlock);
				}
			}
			break;
			case 1:// memfree random memory pool previously allocated by memalloc
			{
				int rand_i = TestStackGetRandom(testStack);
				if (rand_i) {
					TEST_BLOCK blockToFree = TestStackPop(&testStack, rand_i);
					memfree(blockToFree.ptr);
				}
			}
			break;
			}
		}
		// check error of writing over the pool allocated for allocator
		if (CheckOverWrite(ptr, sizeToAlloc))
			return(1);
		FreeAllStack(&testStack);

		// since it is known from design of allocator what int on pointer position 
		// (char *)centre + memgetminimumsize() - memgetblocksize())
		// is total memory avaivable for allocator, whithout memory for global descriptor
		// we can check equation sizeAfterAllocation == sizeBeforeAllocation
		int sizeAfterAllocation = *((int*)((char*)centre + memgetminimumsize() - memgetblocksize()));
		if (sizeAfterAllocation != sizeBeforeAllocation)
			return(1);
		free(ptr);
		memdone();
		CloseFile();
	}
	return(0);
}