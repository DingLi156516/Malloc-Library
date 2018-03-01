This is the simple implementation of malloc library, which has functions like malloc, calloc, realloc, free, reallocarray, memalign, posix_memalign.

In this library I use a list of free blocks that can be used to satisfy a given memory request.Accoding the definition of buddy allocation in Wiki, we can define orders and use the corresponding order to handle the request. When the required size is bigger than 2^9 = 512bytes, we just need to use normal mmap alloction, but if size is smaller than 512, use buddy allocation.



typedef struct MallocHeader {
size_t size;                  //memory size
struct MallocHeader* base;  //base memory block
struct MallocHeader* pre;   //previous memory block
struct MallocHeader* next;  //next memory block
pthread_t id;                //thread id
int order;                     //the order needed for buddy allocation
int FreeOrNot;                //whether this memory block is free
int LargeOrNot;                //whether this memory block is a large block
} MallocHeader;

//MallInfo structure
struct mallinfo{
unsigned long hblkhd;       //blocks of mmaped regions
unsigned long ordblks;        //total blocks
unsigned long uordblks;        //used blocks
unsigned long fordblks;          //free blocks
};

The above two structs are the information I need when I want to implement malloc, for the MallocHeader, it is used to manage memory block, for each block, there are a memory region and the header, so the available memory region is size - sizeof(MallocHeader), Mallinfo is used to get the current information of the malloc, ensure there are no memory leak, and there is also a malloc_stats function which prints out the information about the malloc library.

For the Buddy memory allocation, we use a MallocHeader array to represent the free list it should have 11 blocks(0 - 10), and when a request is smaller than 512 bytes, we use it to first get the order of the block, then extract the free block in the free list, then if there is no space left in the free list, we use sbrk call to ask the kernel for more space and these spaces are allocated by multiple of PAGE_SIZE

For the buddy allocation free function, we first try to find if there are previous free block or next free block which can be used to merge into a bigger block and put it back in to the free list.

For the impelmentation of Per-Core Malloc Arenas, every time we allocate a new bin, we generate the thread id and in the later actions, we will first compare the id with the id stored in MallocHeader, which ensures that it is the right thread. And we use locks to make sure mutual exclusion, the basic structure look like:

pthread_mutex_t lock;
pthread_mutex_lock(&lock);
.... //our malloc function, we should make sure only this critical section is being processed 
pthread_mutex_unlock(&lock);


Although it passes the two test cases, I think it still needs more test to show if it fits all possible situations.

Instructions:
1.make check
2.make clean

