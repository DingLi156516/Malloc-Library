/* This is the header which defines the freelist and initialization of lock */

#include "header.h"
//total used blocks
unsigned long Used_blocks = 0;
//total free blocks
unsigned long Free_blocks = 0;
//total allocation requests
unsigned long Total_allocation_requests = 0;
//total free requests
unsigned long Total_free_requests = 0;
//total number of arenas
unsigned long Total_number_of_arenas = 0;
//the number of mmap requests
unsigned long mmap_requests_count = 0;
//mmap blocks in use
unsigned long mmap_use_count = 0;
//the number of sbrk request
unsigned long sbrk_request_count = 0;
//the number of block we get using sbrk
unsigned long sbrk_block_count = 0;
//sbrk blocks in use
unsigned long sbrk_use_count = 0;
//free mmap requests
unsigned long mmap_free_count = 0;
//free sbrk requests
unsigned long sbrk_free_count = 0;
//freelist
static MallocHeader* FreeHeader[MAX + BASEORDER + 1] = {NULL};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;



