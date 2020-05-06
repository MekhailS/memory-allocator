#pragma warning(disable: 4996)

#include "memallocator.h"

#include <stdlib.h>
#include <stdio.h>

// sizes of descriptors without size var at the end
#define DESC_SIZE sizeof(DESCRIPT)
#define DESC_G_SIZE sizeof(DESCRIPT_G)

// WRITE A COMMENT HERE
//  | size | *next | *prev | memory2allocate | size |


// descriptor for free memory block (without size var at the end)
typedef struct DESCRIPT_T {
	int size;	//size of whole memory block
	struct DESCRIPT_T *next, *prev;	//pointers to next and prev memory blocks in free list
} DESCRIPT;


// descriptor needed for allocator to functionate
typedef struct DESCRIPT_G {
	int memsize;	//total memory avaivable for allocator
	DESCRIPT *freeList;	//head of list of free blocks
}DESCRIPT_G;


// first byte of allocator's memory space
DESCRIPT_G *allocator = NULL;

// file for printing allocator
FILE *fileToPrint = NULL;


void PrintHeadFreeList();
void PrintAllocator();
void PrintAllocatorWHeadFreeList();

int mymin(int a, int b);
int myabs(int a);

char *GetLeftBorder();
char *GetRightBorder();

void MergeWRight(DESCRIPT *block);
DESCRIPT *SplitBlock(DESCRIPT *block, int newSize);
DESCRIPT *SearchBlock(int reqSize);

void SetBlockSize(DESCRIPT *block, int newSize);
void ChangeSizeSign(DESCRIPT *block);

DESCRIPT *GoToLeftNeighbour(DESCRIPT *start);
DESCRIPT *GoToRightNeighbour(DESCRIPT *start);

char *GoToLastSize(DESCRIPT *start);
char *GetMemory(DESCRIPT *start);

void DelFromFreeList(DESCRIPT *block);
void AddToFreeList(DESCRIPT *block);


// open file 'name' for printing allocator
void InitFile(char *name) {
	fileToPrint = fopen(name, "w");
	fprintf(fileToPrint, "format:\n// | size of cur block | size of next block | size of previous block | (-1 if allocated +1 if free) memory | size of cur block |\n\n");
}


// close printing file
void CloseFile() {
	fclose(fileToPrint);
}


// init allocator with memory block 'pMemory' of size 'size'
int meminit(void* pMemory, int size) {
	if ((size > memgetminimumsize()) && (pMemory != NULL)) {
		allocator = (DESCRIPT_G*)pMemory;
		allocator->memsize = size;
		allocator->freeList = ((DESCRIPT*)(((char*)(allocator)) + DESC_G_SIZE));
		allocator->freeList->next = allocator->freeList->prev = allocator->freeList;
		SetBlockSize(allocator->freeList, size - DESC_G_SIZE);
		return(1);
	}
	return(0);
}


// terminate allocator 
void memdone() {
	allocator = NULL;
}


// allocate memory block of size 'sizeReq'
// returns pointer to memory block if success, NULL otherwise
void *memalloc(int sizeReq) {

	/* PRINTING ALLOCATOR */
	if (fileToPrint != NULL) {
		fprintf(fileToPrint, "\n");
		fprintf(fileToPrint, "ALLOCATING BLOCK SIZE OF %d, ALLOCATOR:\n", sizeReq);
		PrintAllocatorWHeadFreeList();
	}
	/* PRINTING ALLOCATOR */

	if ((allocator != NULL) && (sizeReq > 0)) {
		if (SearchBlock(sizeReq) != NULL) {
			// block with memory to return
			DESCRIPT *blockWReqMem = NULL;

			// if too much free memory space — split block on two
			if (allocator->freeList->size >= sizeReq + 2 * (DESC_SIZE + sizeof(int)) + 1) {
				blockWReqMem = SplitBlock(allocator->freeList, DESC_SIZE + sizeof(int) + sizeReq);
			}
			// allocate block
			else {
				blockWReqMem = allocator->freeList;
				DelFromFreeList(blockWReqMem);
			}
			ChangeSizeSign(blockWReqMem);
			blockWReqMem->next = blockWReqMem->prev = NULL;

			/* PRINTING ALLOCATOR */
			if (fileToPrint != NULL) {
				fprintf(fileToPrint, "success alloc, ALLOCATOR:\n", sizeReq);
				PrintAllocatorWHeadFreeList();
			}
			/* PRINTING ALLOCATOR */

			return((void *)(GetMemory(blockWReqMem)));
		}
	}
	/* PRINTING ALLOCATOR */
	if (fileToPrint != NULL) {
		fprintf(fileToPrint, "fail alloc, ALLOCATOR:\n", sizeReq);
		PrintAllocatorWHeadFreeList();
	}
	/* PRINTING ALLOCATOR */

	return(NULL);
}


// free memory previously allocated by memalloc
void memfree(void *memoryToFree) {
	if ((memoryToFree != NULL) && (allocator != NULL)) {
		DESCRIPT *blockToFree = (DESCRIPT *)(((char *)memoryToFree) - DESC_SIZE);

		/* PRINTING ALLOCATOR */
		if (fileToPrint != NULL) {
			fprintf(fileToPrint, "\n");
			fprintf(fileToPrint, "FREE BLOCK SIZE OF %d, ALLOCATOR:\n", blockToFree->size);
			PrintAllocatorWHeadFreeList();
		}
		/* PRINTING ALLOCATOR */

		ChangeSizeSign(blockToFree);
		MergeWRight(blockToFree);
		AddToFreeList(blockToFree);

		//if left neighbour free, merge with left
		if (GoToLeftNeighbour(blockToFree) != NULL) {
			if ((GoToLeftNeighbour(blockToFree))->size > 0) {
				DESCRIPT *mergedBlock = GoToLeftNeighbour(blockToFree);
				MergeWRight(mergedBlock);
			}
		}
	}

	/* PRINTING ALLOCATOR */
	if (fileToPrint != NULL) {
		fprintf(fileToPrint, "success free, ALLOCATOR:\n");
		PrintAllocatorWHeadFreeList();
	}
	/* PRINTING ALLOCATOR */

}


// returns size in bytes of additional memory per allocation
// size of descriptor (full descriptor)
int memgetblocksize() {
	return(DESC_SIZE + sizeof(int));
}


// return minimum size in bytes of the memory pool to allocate 0-bytes block
// size of minimum memory for allocator to functionate
int memgetminimumsize() {
	return(DESC_G_SIZE + DESC_SIZE + sizeof(int));
}


// get left limit border of space for allocation
char *GetLeftBorder() {
	return((char*)(allocator + 1));
}


// get right limit border of space for allocation
char *GetRightBorder() {
	return(((char*)allocator) + allocator->memsize);
}


// find first fit free block with enough memory to allocate
DESCRIPT *SearchBlock(int reqSize) {
	DESCRIPT *cyclePoint = allocator->freeList;
	// traverse through free list
	if (allocator->freeList != NULL) {
		do {
			if (allocator->freeList->size >= reqSize + DESC_SIZE + sizeof(int))
				return(allocator->freeList);
			else
				allocator->freeList = allocator->freeList->next;
		} while (allocator->freeList != cyclePoint);
	}
	return(NULL);
}


// split the memory block 'block' on two, with right block of size 'rightSize'
DESCRIPT *SplitBlock(DESCRIPT *block, int rightSize) {
	int totalSize = block->size;
	if (rightSize + DESC_SIZE + sizeof(int) + 1 <= totalSize) {
		SetBlockSize(block, totalSize - rightSize);
		SetBlockSize(GoToRightNeighbour(block), rightSize);
		GoToRightNeighbour(block)->next = NULL;
		GoToRightNeighbour(block)->prev = NULL;
		return(GoToRightNeighbour(block));
	}
	return(NULL);
}


// set the size of block
void SetBlockSize(DESCRIPT *block, int newSize) {
	block->size = newSize;
	*((int*)(GoToLastSize(block))) = newSize;
}


// get ptr to the memory of block (avaivable for allocation to user)
char *GetMemory(DESCRIPT *block) {
	return(((char *)block) + DESC_SIZE);
}


// move to left 'neighbour' memory block of current block
DESCRIPT *GoToLeftNeighbour(DESCRIPT *block) {
	if (((char*)block) <= GetLeftBorder())
		return(NULL);
	int size = myabs(*((int *)(((char*)(block)) - sizeof(int))));
	if ((((char *)block) - size) < GetLeftBorder())
		return(NULL);
	return((DESCRIPT *)(((char *)block) - size));
}


// move to right 'neighbour' memory block of current block
DESCRIPT *GoToRightNeighbour(DESCRIPT *block) {
	int size = myabs(block->size);
	if ((((char *)block) + size) >= GetRightBorder())
		return(NULL);
	return((DESCRIPT *)(((char *)block) + size));
}


// get pointer to size at the end of memory block
char *GoToLastSize(DESCRIPT *block) {
	int size = myabs(block->size);
	return(((char *)block) + size - sizeof(int));
}


// delete memory block from list of free blocks
void DelFromFreeList(DESCRIPT *block) {
	if (allocator->freeList != NULL) {
		// if block is head of free list
		if (block == allocator->freeList)
			allocator->freeList = allocator->freeList->next;
		// if free list contains only this block
		if (block->next == block)
			allocator->freeList = NULL;

		block->next->prev = block->prev;
		block->prev->next = block->next;
		block->next = block->prev = NULL;
	}
}


// add a memory block ad a head of free list
void AddToFreeList(DESCRIPT *block) {
	// if free list is not empty
	if (allocator->freeList != NULL) {
		block->next = allocator->freeList;
		block->prev = allocator->freeList->prev;
		allocator->freeList->prev = block;
		block->prev->next = block;
		allocator->freeList = block;
	}
	// if free list is empty
	else {
		allocator->freeList = block;
		block->next = block;
		block->prev = block;
	}
}


// change the size's sign of memory block 
// (- if not reserved, + if free)
void ChangeSizeSign(DESCRIPT *block) {
	*((int*)(GoToLastSize(block))) *= -1;
	block->size *= -1;
}


// merge free memory block with its right neighbour
void MergeWRight(DESCRIPT *block) {
	DESCRIPT *rightBlock = GoToRightNeighbour(block);
	if (((char *)rightBlock) != NULL) {
		// if right block is reserved
		if (rightBlock->size < 0)
			return;
		DelFromFreeList(rightBlock);
		int newSize = rightBlock->size + block->size;
		SetBlockSize(block, newSize);
	}
}


// function to print memory block into 'fileToPrint'
void PrintMemoryBlock(DESCRIPT *block) {
	int signBlock = myabs(block->size) / block->size;
	int nextSize = 0;
	int prevSize = 0;
	if (block->next != NULL)
		nextSize = block->next->size;
	if (block->prev != NULL)
		prevSize = block->prev->size;
	fprintf(fileToPrint, "| %d | %d | %d |%d memory| %d |", block->size, nextSize, prevSize, signBlock, *((int *)GoToLastSize(block)));
	// | size of cur block | size of next block | size of previous block | (-1 if allocated +1 if free) memory | size of cur block |
}


// function to print specially head of free list
void PrintHeadFreeList() {
	if (allocator->freeList == NULL)
		fprintf(fileToPrint, "HEAD IS NULL");
	else {
		fprintf(fileToPrint, "HEAD IS: ");
		PrintMemoryBlock(allocator->freeList);
	}
}


// function to print full allocator by blocks
void PrintAllocator() {
	DESCRIPT *block = (DESCRIPT *)(allocator + 1);
	while (1){
		PrintMemoryBlock(block);
		if (GoToRightNeighbour(block) != NULL) {
			block = GoToRightNeighbour(block);
		}
		else
			break;
	}
}


// function to print full allocator by blocks with head of free list
void PrintAllocatorWHeadFreeList() {
	PrintHeadFreeList();
	fprintf(fileToPrint, "\n");
	PrintAllocator();
	fprintf(fileToPrint, "\n");
}


int myabs(int a) {
	if (a > 0)
		return(a);
	return(-a);
}


int mymin(int a, int b) {
	if (a > b)
		return(b);
	return(a);
}