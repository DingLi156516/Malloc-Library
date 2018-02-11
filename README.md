This is the simple implementation of malloc library, which has functions like malloc, calloc, realloc, free, reallocarray, memalign, posix_memalign.

In this library I use a list of free blocks that can be used to satisfy a given memory request.Accoding the definition of buddy allocation in Wiki, we can define orders and use the corresponding order to handle the request. When the required size is bigger than 2^9 = 512bytes, we just need to use normal mmap alloction, but if size is smaller than 512, use buddy allocation.

Although it passes the two test cases, I think it still needs more test to show if it fits all possible situations.

Instructions:
1.make check
2.make clean
