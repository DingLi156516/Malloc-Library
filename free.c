/* This is the free function */

#include "header.h"

void free(void *ptr)
{
  pthread_mutex_lock(&lock);

  MyFree(ptr);

  pthread_mutex_unlock(&lock);
}

