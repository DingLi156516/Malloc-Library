#include "thread.h"

//malloc_stats which prints out the information of this malloc family
void malloc_stats() {
	Myprintf("Malloc Statistics\n\n");
	Myprintf("Total Allocattion Requests: %lu\n", Total_allocation_requests);
	Myprintf("Total Free Requests: %lu\n", Total_free_requests);
	Myprintf("Total Blocks in Use: %lu\n", mmap_use_count + sbrk_use_count);
	Myprintf("Total Free Blocks: %lu\n", sbrk_free_count + mmap_free_count);
	Myprintf("Total Number of Blocks: %lu\n", mmap_requests_count + sbrk_block_count);

	Myprintf("Mmap Requests Count: %lu\n", mmap_requests_count);
	Myprintf("Mmap Block in Use: %lu\n", mmap_use_count);
	Myprintf("Mmap Block Free Count: %lu\n", mmap_free_count);

	Myprintf("Sbrk Requests Count: %lu\n", sbrk_request_count);
	Myprintf("Sbrk Block Count: %lu\n", sbrk_block_count);
	Myprintf("Sbrk Block In Use: %lu\n", sbrk_use_count);
	Myprintf("Sbrk Block Free Count: %lu\n", sbrk_free_count);
}
//mallinfo function which stores the current malloc library status
struct mallinfo mallinfo(){
	struct mallinfo info = {0,0,0,0};
	info.hblkhd += mmap_requests_count;
	info.ordblks += mmap_requests_count + sbrk_block_count;
	info.uordblks += mmap_use_count + sbrk_use_count;
	info.fordblks += sbrk_free_count + mmap_free_count;
	return info;
}

//since we can't use printf, it is better to have our own printf function
void Myprintf(const char * format, ...)  
{   
    va_list list;  
    va_start(list, format);  
    char buff[4096];
    vsnprintf(buff, 4096, format, list); 
    write(STDOUT_FILENO, buff, strlen(buff));
    va_end(list);   
} 
// malloc implementation
void *Mymalloc(size_t size){
	Total_allocation_requests++;
	void *res = NULL;
	//LARGE is the boundary between using normal mmap and buddy allocation
	if (size > LARGE)
	{
		res = LARGE_MALLOC(size);
	}
	else
	{
		res = SMALL_MALLOC(size);
	}

	if(res == NULL)
	{
		Myprintf("Mymalloc error\n");
	}
	return res;
	
}

void *LARGE_MALLOC(size_t size){
	size_t allocSize = size + sizeof(MallocHeader);

  	void *res = mmap(0, allocSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  	if (res == MAP_FAILED || res < 0)
  	{
  		Myprintf("Malloc error, mmap\n");
  		return NULL;
  	}
	mmap_requests_count++;
	mmap_use_count++;
  	MallocHeader *hdr = (MallocHeader*) res;
  	hdr->size = allocSize;
  	hdr->id = pthread_self();
  	hdr->order = 0;
  	hdr->FreeOrNot = 0;
  	hdr->LargeOrNot = 1;
  	return (void *)(++hdr);
}


void *SMALL_MALLOC(size_t size){
	size_t allocSize = size + sizeof(MallocHeader);
	//get the current order
	int order = 0;
	int buddysize = 0;
	int i = 0;
	for(i = MIN; i <= MAX; ++i) {
		order = i;
		buddysize = BASE * (1 << i);
		if (buddysize >= allocSize) {
			break;
		}
	}
	order += BASEORDER;
	MallocHeader *hdr = GetBlock(order);
	if (hdr == NULL)
	{
		MallocBlock(order);
		hdr = GetBlock(order);
	}
	//delete hdr from the freelist
	delete_h(&FreeHeader[order], hdr);
	hdr->FreeOrNot = 0;
	sbrk_use_count++;

	return (void *)(++hdr);

}
//take the block which has the current order 
MallocHeader *GetBlock(int order){
	MallocHeader *header = FreeHeader[order];
	pthread_t id = pthread_self();
	while(header != NULL && header->id != id){
		header = header->next;
	}
	return header;
}
//when there is no free memory, ask kernel for more memory
void NewMemory(){
	size_t allocSize = NEW_PAGE * sysconf(_SC_PAGESIZE);
	void *res = sbrk(allocSize);
	if(res == (void *)-1)
	{
		Myprintf("error in new NewMemory sbrk\n");
		return;
	}

	int count = allocSize / ((1 << MAX) * BASE);
	MallocHeader *hdr = (MallocHeader *)(res + ((1 << MAX) * BASE) * (count - 1));
	MallocHeader *last = hdr;
	hdr->id = pthread_self();
	hdr->size = (1 << MAX) * BASE;
	hdr->order = MAX + BASEORDER;
	hdr->FreeOrNot = 1;
	hdr->LargeOrNot = 0;
	hdr->base = (MallocHeader *) res;
	hdr->next = FreeHeader[MAX + BASEORDER];
	int i;
	for(i = 0; i < count - 1; ++i)
	{
		MallocHeader *Next = hdr;
		hdr = (MallocHeader *)((void *)hdr - ((1 << MAX) * BASE));
		memcpy(hdr, Next, sizeof(MallocHeader));
		hdr->next = Next;
		Next->pre = hdr;
		/* code */
	}
	hdr->pre = NULL;
	if (FreeHeader[MAX + BASEORDER] != NULL)
	{
		FreeHeader[MAX + BASEORDER]->pre = last;
		/* code */
	}
	FreeHeader[MAX + BASEORDER] = hdr;
	sbrk_block_count += count;

}

void MallocBlock(int order){
	int s = order;
	MallocHeader *hdr = NULL;
	hdr = GetBlock(s);
	while(s <= (MAX + BASEORDER) && hdr == NULL){
		hdr = GetBlock(s++);
	}

	if(hdr == NULL){
		NewMemory();
		hdr = GetBlock(MAX + BASEORDER);
	}

	for (s = hdr->order; s > order; --s)
	{
		hdr = GetBlock(s);

		delete_h(&FreeHeader[s], hdr);

		MallocHeader *shdr = (MallocHeader *)((void *)hdr + (1<< (s - 1)));
		hdr->order = s - 1;
		hdr->size = 1 << (s - 1);
		hdr->pre = NULL;
		hdr->next = shdr;
		memcpy(shdr, hdr, sizeof(MallocHeader));
		shdr->pre = hdr;
		shdr->next = FreeHeader[s - 1];
		if (FreeHeader[s - 1] != NULL)
		{
			FreeHeader[s - 1]->pre = shdr;
		}
		FreeHeader[s - 1] = hdr;
	}
}

void MyFree(void *ptr){
	Total_free_requests++;
	if (ptr == NULL)
	{
		return;
		/* code */
	}
	MallocHeader *hdr = ptr - sizeof(MallocHeader);
  	// We can't use printf here because printf internally calls `malloc` and thus
  	// we'll get into an infinite recursion leading to a segfault.
  	// Instead, we first write the message into a string and then use the `write`
  	// system call to display it on the console.
  	pthread_t id = pthread_self();
  	if(hdr->id != id)
  	{
  		Myprintf("bad request to release memory not belong to the current thread\n");
  		return;
  	}
  	if (hdr->LargeOrNot)
  	{
  		LARGE_FREE(hdr);
  		/* code */
  	}
  	else
  	{
  		SMALL_FREE(hdr);
  	}
  	
}
//free the large block
void LARGE_FREE(MallocHeader *hdr){
	int res = munmap(hdr, hdr->size);
	if(res < 0)
	{
		Myprintf("error in munmap\n");
	}
	mmap_use_count--;
	mmap_free_count++;
}
//free the block that is smaller than LARGE
void SMALL_FREE(MallocHeader *hdr){
	if (hdr->FreeOrNot == 1)
	{
		Myprintf("this block is free already\n");
		return;
	}

	hdr->FreeOrNot = 1;
	int s = hdr->order;
	int i;
	for (i = s; i < (MAX + BASEORDER); ++i)
	{
		MallocHeader *Next = NULL;
		if (hdr > hdr->base)
		{
			Next = (void *)hdr - (1 << i);
			if (Next->id == hdr->id && Next->order == i && !Next->LargeOrNot && Next->FreeOrNot)
			{
				delete_h(&FreeHeader[i], Next);
				Next->order = i + 1;
				Next->size = 1 << (i + 1);
				hdr->order = 0;
				hdr->FreeOrNot = 0;

				hdr = Next;
				continue;
			}
		}

		if ((void *)hdr < (void *)hdr->base + ((1 << MAX)*BASE))
		{
			Next = (void *)hdr + (1 << i);
			if (Next->id == hdr->id && Next->order == i && !Next->LargeOrNot && Next->FreeOrNot)
			{
				delete_h(&FreeHeader[i], Next);
				Next->order = 0;
				Next->FreeOrNot = 0;
				hdr->order = i + 1;
				hdr->size = 1 << (i + 1);

				continue;
			}
		}
		break;
	}
	s = hdr->order;
	insert(&FreeHeader[s], hdr);
	sbrk_use_count--;
	sbrk_free_count++;
}

void delete_h(MallocHeader **header, MallocHeader *hdr){
	MallocHeader* pre = hdr->pre;
	MallocHeader* next = hdr->next;
	if (pre == NULL)
	{
		*header = next;
	}
	else
	{
		pre->next = next;
	}
	if (next != NULL)
	{
		next->pre = pre;
	}
}

void insert(MallocHeader **header, MallocHeader *hdr){
	hdr->next = *header;
	hdr->pre = NULL;
	if (*header != NULL)
	{
		(*header)->pre = hdr;
	}
	*header = hdr;
}






