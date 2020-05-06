#pragma once


// maximum number of spaces allocated by memalloc at time
#define TEST_N 5
// memory pool for override check
#define MEMORY_N 21
// average size of memory pool to allocate by memalloc
#define AVG_MEMBLOCK_SIZE 50
// total number of tests
#define TEST_TRY_N 1


// function perfoms integration test of allocator on random input
// randomly allocates and frees the memory blocks of avg size AVG_MEMBLOCK_SIZE
// 3*TEST_N times, after that frees all allocated memory by memfree()
// iterates this TEST_TRY_N times (TEST_TRY_N - max number of allocated spaces by memalloc())
// checks override, errors in memfree() and memalloc()
// 0 if no error occured, 1 — otherwise
int FullComplexTest(char *fileName);