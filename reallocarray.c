/* This is the reallocarray function */

#include "header.h"

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	pthread_mutex_lock(&lock);
	void *res = ptr;

	if (ptr == NULL && (size == 0 || nmemb == 0)) {
		Myprintf("size is 0\n");
		pthread_mutex_unlock(&lock);
		return res;
	}

  if (ptr == NULL) {
    size_t TotalSize = nmemb * size;
  	res = Mymalloc(TotalSize);
  	pthread_mutex_unlock(&lock);
  	return res;
  }

  if (size == 0 || nmemb == 0) {
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
  size_t TotalSize = nmemb * size;
  if (curSize != TotalSize) {
  	res = Mymalloc(TotalSize);
  	size_t copySize = TotalSize > curSize ? curSize : TotalSize;
  	memcpy(res, ptr, copySize);
  	MyFree(ptr);
  }

	pthread_mutex_unlock(&lock);
  	return res;
}


