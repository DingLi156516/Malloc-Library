/* This is the calloc function */
#include "header.h"

void *malloc(size_t size)
{
	pthread_mutex_lock(&lock);

	void *res = Mymalloc(size);

	pthread_mutex_unlock(&lock);
	return res;
}


