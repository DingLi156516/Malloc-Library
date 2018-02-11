/* This is the realloc function */

#include "header.h"

void *realloc(void *ptr, size_t size)
{
	pthread_mutex_lock(&lock);
	void *res = ptr;

	if (ptr == NULL && size == 0) {
		Myprintf("size is 0\n");
		pthread_mutex_unlock(&lock);
		return res;
	}

  if (ptr == NULL) {
  	res = Mymalloc(size);
  	pthread_mutex_unlock(&lock);
  	return res;
  }

  if (size == 0) {
  	MyFree(ptr);
  	pthread_mutex_unlock(&lock);
  	return ptr;
  }

  	MallocHeader *hdr = ptr - sizeof(MallocHeader);
  	pthread_t id = pthread_self();
  	if (hdr->id != id) {
  		Myprintf("a bad request to release memory not belong to the current thread\n");
  		pthread_mutex_unlock(&lock);
  		return res;
  	}

  size_t curSize = hdr->size - sizeof(MallocHeader);
  if (curSize != size) {
  	res = Mymalloc(size);
  	size_t copySize = size > curSize ? curSize : size;
  	memcpy(res, ptr, copySize);
  	MyFree(ptr);
  }

	pthread_mutex_unlock(&lock);
  	return res;
}


