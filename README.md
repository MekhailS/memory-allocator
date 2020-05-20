# First-fit memory allocator
### Memory allocation system, based on first-fit allocation scheme. Written on C language
+ Implemented memory allocation functions: `meminit`, `memalloc`, `memfree`, `memdone`, `memgetminimumsize`, `memgetblocksize`
+ All non-trivial functions are covered with unit tests, see [test.cpp](https://github.com/MekhailS/memory-allocator/blob/master/memallocator-gtest/test.cpp) 
+ Integration tests are performed in [fullcomplex-test.c](https://github.com/MekhailS/memory-allocator/blob/master/memory-allocator/fullcomplex-test.c) 
+ System supports printing of allocator's current state, see example [log.alloclog](https://github.com/MekhailS/memory-allocator/blob/master/memory-allocator/log.alloclog)
