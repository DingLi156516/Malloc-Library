/* This is the header which defines all the functions and struct we used in this malloc library*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <pthread.h>
//this is the base which is 2^6
#ifndef BASE
#define BASE 64
#endif

#ifndef MIN
#define MIN 0
#endif

#ifndef MAX
#define MAX 4
#endif
//this is the baseorder that we need to use
#ifndef BASEORDER
#define BASEORDER 6
#endif
//define the boundary between normal mmap and buddy allocation
#ifndef LARGE
#define LARGE 512
#endif

#ifndef NEW_PAGE
#define NEW_PAGE 4
#endif

typedef struct MallocHeader {
	size_t size;  				//memory size
	struct MallocHeader* base;  //base memory block
	struct MallocHeader* pre;   //previous memory block
    struct MallocHeader* next;  //next memory block
    pthread_t id;				//thread id
	int order; 					//the order needed for buddy allocation
    int FreeOrNot;				//whether this memory block is free
    int LargeOrNot;				//whether this memory block is a large block
} MallocHeader;

//MallInfo structure
struct mallinfo{
	unsigned long hblkhd;       //blocks of mmaped regions
	unsigned long ordblks;		//total blocks
	unsigned long uordblks;		//used blocks 
	unsigned long fordblks;	  	//free blocks 
};

//this is the list which defines all the functions
void *Mymalloc(size_t size);
void Myprintf(const char * format, ...);
void *LARGE_MALLOC(size_t size);
void *SMALL_MALLOC(size_t size);
MallocHeader *GetBlock(int order);
void NewMemory();
void MallocBlock(int order);
void MyFree(void *ptr);
void LARGE_FREE(MallocHeader *hdr);
void SMALL_FREE(MallocHeader *hdr);
void delete_h(MallocHeader **header, MallocHeader *hdr);
void insert(MallocHeader **header, MallocHeader *hdr);
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void *reallocarray(void *ptr, size_t nmemb, size_t size);
void *memalign(size_t alignment, size_t size);
void malloc_stats();
struct mallinfo mallinfo();
extern pthread_mutex_t lock;





