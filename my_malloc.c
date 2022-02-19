//
//  my_malloc.c
//  Lab1: Malloc
//
#include <unistd.h>
#include <stdio.h>
#include "my_malloc.h"

#define CHUNK_SIZE (8192)
#define MAGIC 0x77A5844CU

MyErrorNo my_errno=MYNOERROR;

FreeListNode head;

void insert_node(FreeListNode node);
FreeListNode remove_node(uint32_t size);

void *my_malloc(uint32_t size)
{
	static void* free_heap_begin = NULL;
	void* ptr;
	void* heap_end;
	char* temp;
	char* new_start;
	uint32_t heap_size;
	if (size == 0) {
		return NULL;
	}
	if (!free_heap_begin) {
		free_heap_begin = sbrk(0);
	}
	size += CHUNKHEADERSIZE;
	if (size < (CHUNKHEADERSIZE * 2)) {
		size = (CHUNKHEADERSIZE * 2);
	}
	while (size % CHUNKHEADERSIZE != 0) {
		size++;
	}
	// Find Chunk from free list
	FreeListNode free_chunk = remove_node(size);
	if (free_chunk) {
		int original_size = free_chunk->size;
		temp = (char*)free_chunk;
		*((int*)temp) = size;
		temp += (CHUNKHEADERSIZE / 2);	
		*((int*)temp) = MAGIC;
		temp += (CHUNKHEADERSIZE / 2);
		ptr = (void*)temp;	
		if (original_size == size) {	
			return ptr;
		}
		new_start = (char*)free_chunk;
		new_start += size;
		FreeListNode node = (FreeListNode)new_start;
		node->flink = NULL;
		node->size = original_size - size;
		insert_node(node);		
		return ptr;	
	}
	temp = (char*)free_heap_begin;	
	heap_end = sbrk(0); 	
	if (free_heap_begin + size > heap_end) {	
		if (free_heap_begin + size - heap_end > CHUNK_SIZE) {
			heap_size = free_heap_begin + size - heap_end;
		} else {
			heap_size = CHUNK_SIZE;
		}	
		if (sbrk(heap_size) == -1) {
			my_errno = MYENOMEM;
			return NULL;
		}		
	}	
	*((int*)temp) = size;
	temp += (CHUNKHEADERSIZE / 2);
	*((int*)temp) = MAGIC;
	temp += (CHUNKHEADERSIZE / 2);
	free_heap_begin += size;	
	if (sbrk(0) > free_heap_begin) {
		free_chunk = (FreeListNode)free_heap_begin;
		free_chunk->flink = NULL;
		free_chunk->size = heap_size - size;
	       	insert_node(free_chunk);
		free_heap_begin = sbrk(0);
	}	
	ptr = (void*)temp;	
	return ptr;	

}
      
void my_free(void *ptr)
{
	char* temp;
	uint32_t size;
	int flag;
	FreeListNode node;
	if (ptr == NULL) {
		my_errno = MYBADFREEPTR;
		return;
	}
	temp = (char*)ptr;
	temp -= (CHUNKHEADERSIZE / 2);
	flag = *((int*)temp);
	if (flag != MAGIC) {
		my_errno = MYBADFREEPTR;
		return;
	}
	*((int*)temp) = 0;
	temp -= (CHUNKHEADERSIZE / 2);
	size = *((int*)temp);
	node = (FreeListNode)temp;
	node->flink = NULL;
	node->size = size;
	insert_node(node);	
}

FreeListNode free_list_begin()
{
	return head;
}

void coalesce_free_list()
{
	FreeListNode temp = head;
	while (temp && temp->flink) {
		char* first = (char*) temp;
		char* second = (char*) temp->flink;
		uint32_t first_size = temp->size;
		uint32_t second_size = temp->flink->size;
		if (first + first_size >= second) {
			char* second_end = second + second_size - 1;
			temp->size = second_end - first + 1;	
			temp->flink = temp->flink->flink;
			continue;
		}
		temp = temp->flink;
	}
}

void insert_node(FreeListNode node)
{
	FreeListNode slow;
	FreeListNode fast;
	slow = head;
	fast = slow;
	if (head == NULL) {
		node->flink = NULL;
		head = node;
		return;
	}
	if (head > node) {
		node->flink = head;
		head = node;	
		return;
	}
	fast = head->flink;
	while (fast && (fast < node)) {	
		slow = fast;
		fast = slow->flink;
	}
	slow->flink = node;
	node->flink = fast;	
}

FreeListNode remove_node(uint32_t size)
{
	FreeListNode slow = head;
	if (slow == NULL) {
		return NULL;
	}
	if (slow->size >= size) {
		FreeListNode temp = slow;
		head = slow->flink;
		temp->flink = NULL;
		return temp;
	}
	FreeListNode fast = slow->flink;
	while (fast) {
		if (fast->size >= size) {
			FreeListNode temp = fast;
			fast = fast->flink;
			slow->flink = fast;
			temp->flink = NULL;
			return temp;
		}
		slow = fast;
		fast = slow->flink;
	}
	return NULL;
}
