/* This is the header which defines the freelist and initialization of lock */

#include "header.h"
//freelist
static MallocHeader* FreeHeader[MAX + BASEORDER + 1] = {NULL};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


