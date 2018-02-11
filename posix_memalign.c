/* This is the posix_memalign function */

#include "header.h"

int posix_memalign(void **memptr, size_t alignment, size_t size){
	pthread_mutex_lock(&lock);
	void *ptr = NULL;
	int error = 0;
	//We want it to be a power of two since align_up operates on powers of two
	if (alignment == 0 || (alignment & (sizeof (void *) - 1)) != 0 || (alignment & (alignment - 1)) != 0){
		error = EINVAL;
		pthread_mutex_unlock(&lock);
		return (error);

	}
	else if (size != 0 && (ptr = memalign(alignment, size)) == NULL){
		error = ENOMEM;
		pthread_mutex_unlock(&lock);
		return (error);
	}

	*memptr = ptr;
	pthread_mutex_unlock(&lock);
	return (error);

}

