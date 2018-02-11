/* This is the calloc function */

#include "header.h"

void *calloc(size_t nmemb, size_t size)
{
	pthread_mutex_lock(&lock);
	if (nmemb == 0 || size == 0) {
		pthread_mutex_unlock(&lock);
		return NULL;
	}
	size_t TotalSize = nmemb * size;
	void* res = Mymalloc(TotalSize);
	if (res != NULL) {
		memset(res, 0, TotalSize);
	}

	pthread_mutex_unlock(&lock);
  	return res;
}


