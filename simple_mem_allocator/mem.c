/*
 * A simple memory allocator (malloc(), calloc(), realloc() and free())
 *
 * This is not very fast or efficient, and does not adjust allocation to
 * align with page boundaries but it works
 *
 * In a process, the virtual memory table allocates text (instructions), 
 * data (initialized variables), bss (uninitialized variables), and heap
 * starting from memory address 0x0 and growing upwards.  All sections are
 * static size except for the heap which grows upwards.  The stack starts at
 * the top of the address space and grows down.
 *
 * To keep track of the heap, we use a break pointer referred to as brk that
 * keeps track of the top most address in the heap.  Allocating more memory
 * is essentially just requesting the system to increment the brk pointer, and
 * then allocate a page from the free list to our process and map it in the tlb
 * & page table.  To release memory, we request the system to decrement brk. 
 *
 * glibc still uses sbrk() family of system calls for allocating memory 
 * in the case of small memory allocations.  sbrk() is not thread-safe, and
 * more modern tools such as mmap() are far more suited for memory allocation,
 * but for this simple implementation we will use sbrk()
 *
 * To build as a library, use gcc -o mem.so -fPIC -shared mem.c
 * fPIC tells the compiler to build position independent code, and shared tells
 * it to build a binary compatible with dynamic linking.
 *
 * On Linux, if you set the enivornment variable LD_PRELOAD to the path of a 
 * shared object, that file will be loaded before any other library. We could 
 * use this trick to load our compiled library file first, so that the later 
 * commands run in the shell will use our malloc(), free(), calloc() and realloc().
 * $ export LD_PRELOAD=$PWD/mem.so
 */

#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <stdio.h>	// for testing


/* 
 * Stored in the header of every block of allocated memory, this allows us
 * to track the amount of memory for when we need to free the memory.  We
 * use the is_free status to release memory in the middle of the heap without
 * actually freeing it and having to copy memory down into earier addresses.
 *
 * The next pointer is used when freeing memory to point to the next block
 * of allocated memory.  This is necessary because our malloc may not return
 * contiguous memory in cases where blocks are already mmap()'d or some other
 * scenarios.
 */
struct header_t {
	size_t size;
	unsigned is_free;
	struct header_t* next;
};

/* pointers to the head and tail of the memory linked list */
struct header_t *head, *tail;

/* lock used to limit access to memory linked list */
pthread_mutex_t global_malloc_lock;

/* 
 * traverse the block linked list to check if there is a released block that
 * we can use to fulfill the request.
 */
struct header_t *get_free_block(size_t size) {
	struct header_t *cur = head;
	while (cur) {
		if (cur->is_free && cur->size >= size) 
			return cur;
		cur = cur->next;
	}

	return NULL;
}

/*
 * This function takes the amount of memory that you would like to request
 * from the system, in number of bytes and returns a pointer to the beginning
 * of the memory segment.
 */
void *malloc(size_t size) 
{
	size_t total_size;
	struct header_t *header;
	void *block;

	if (!size)
		return NULL;

	pthread_mutex_lock(&global_malloc_lock);
	
	/* if we have a free block already alloc'd to this process, then grab it */
	header = get_free_block(size);
	if (header) {
		header->is_free = 0;
		pthread_mutex_unlock(&global_malloc_lock);
		return (void *)(header+1);
	}

	/* else if there is not space for us */
	total_size = size + sizeof(struct header_t);
	block = sbrk(total_size);
	if (block == (void *) -1) {
		pthread_mutex_unlock(&global_malloc_lock);
		return NULL;
	}

	/* else setup a new block */
	header = block;
	header->size = size;
	header->is_free = 0;
	header->next = NULL;
	if (!head)
		head = header;
	if (tail)
		tail->next = header;
	tail = header;
	pthread_mutex_unlock(&global_malloc_lock);
	return (void *)(header + 1);

	return block;
}

/*
 * This function takes a pointer to the block of memory that you would like
 * to release. If it is at the top of the stack then we free it and decrement
 * brk, if it is not then we mark it as released and hope that it may be used
 * at some point in the future.
 */
void free(void *block) 
{
	struct header_t *header, *tmp;
	void *programbreak;

	if (!block) 
		return;

	pthread_mutex_lock(&global_malloc_lock);
	header =(struct header_t*) block - 1;

	programbreak = sbrk(0);	// get pointer to top of brk
	/* if the top of our block is the top of the heap */
	if ((char*)block + header->size == programbreak) {
		if (head == tail) {
			head = tail = NULL;
		} else {
			tmp = head;
			while (tmp) {
				if (tmp->next == tail) {
					tmp->next = NULL;
					tail = tmp;
				}
				tmp = tmp->next;
			}
		}
		/* set new brk to current pos - block size */
		sbrk(0 - sizeof(struct header_t) - header->size);
		pthread_mutex_unlock(&global_malloc_lock);
		return; 
	}

	header->is_free = 1;
	pthread_mutex_unlock(&global_malloc_lock);
}

/*
 * calloc allocates memory for an array of num elements of size nsize and
 * returns a pointer to the allocated memory.  Additionally, calloc 
 * initializes the memory to 0
 */
void *calloc(size_t num, size_t nsize)
{
	size_t size;
	void *block;

	if (!num || !nsize) 
		return NULL;

	size = num * nsize;

	/* check for multiplication overflow */
	if (nsize != (size/num))
		return NULL;

	block = malloc(size);
	if (!block) 
		return NULL;

	memset(block, 0, size);
	return block;
}

/*
 * realloc changes the memory size of a given block
 */
void *realloc(void *block, size_t size)
{
	struct header_t *header;
	void *ret;
	if (!block || !size) 
		return malloc(size);

	header = (struct header_t*)block - 1;
	if (header->size >= size) 
		return block;

	ret = malloc(size);
	if (ret) {
		memcpy(ret, block, header->size);
		free(block);
	}
	return ret;
}

