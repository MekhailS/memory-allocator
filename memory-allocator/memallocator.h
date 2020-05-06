#pragma once

//NEXT-FIT MEMORY ALLOCATOR


// init allocator with memory block 'pMemory' of size 'size'
int meminit(void *pMemory, int size);


// terminate allocator 
void memdone();


// allocate memory block of size 'size'
// returns pointer to memory block is success, 0 otherwise
void *memalloc(int size);


// free memory previously allocated by memalloc
void memfree(void * p);


// return minimum size in bytes of the memory pool to allocate 0-bytes block
// use case:
// void *p   = 0;
// void *ptr = malloc(memgetminimumsize() + 1);
// meminit(ptr, memgetminimumsize() + 1)
// p = memalloc(1); // Success!
// memfree(p);
// memdone();
// free(ptr);
int memgetminimumsize();


// returns size in bytes of additional memory per allocation
// use case:
// void *p1 = 0, *p2 = 0;
// int  memsize = memgetminimumsize() + memgetblocksize() + 2;
// void *ptr = malloc(memsize);
// meminit(ptr, memsize())
// p1 = memalloc(1); // Success!
// p2 = memalloc(1); // Success!
// memfree(p2);
// memfree(p1);
// memdone();
// free(ptr);
int memgetblocksize();


// open file 'name' for printing allocator
void InitFile(char* name);


// close printing file
void CloseFile();
