/* This is the memalign function */
#include "header.h"

void *memalign(size_t alignment, size_t size)
{
	pthread_mutex_lock(&lock);

	void * ptr = NULL;
	

    //We want it to be a power of two since align_up operates on powers of two
    if((alignment & (alignment - 1)) != 0 || alignment == 0)
    {
    	Myprintf("alignment is not as expected\n");
    	pthread_mutex_unlock(&lock);
		return ptr;
    }
    //Size can't be 0
    if (size == 0)
    {
    	Myprintf("size is 0\n");
		pthread_mutex_unlock(&lock);
		return ptr;
    }
    /*if the alignment is greater than size, then we just need to malloc the size of alignment since 
    it must have enough space for allocate "size" bytes and satisify the alignment, if alignment is smaller than size,
    then we just need to malloc the size of "size" since our function will satisify the alignment if alignment is smaller 
    than size
    */
    if (alignment <= size)
    {
    	ptr = Mymalloc(size);
    }
    else
    {
    	ptr = Mymalloc(alignment);

    }
	pthread_mutex_unlock(&lock);
	return ptr;
}


