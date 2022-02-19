//
//  my_malloc-driver.c
//  Lab1: Malloc
//
#include <stdio.h>

#include "my_malloc.h"

int main(int argc, const char * argv[])
{
	
	void* a = my_malloc(50);
	void* b = my_malloc(100);
	void* c = my_malloc(3000);
	my_free(a);
	my_free(b);
	my_free(c);
	FreeListNode temp;	
	void* d = my_malloc(10000);
	my_free(d);
	a = my_malloc(30);
	temp = free_list_begin();
	while (temp) {
		uint32_t size = temp->size;
		temp = temp->flink;
	}
	coalesce_free_list();	
    	return 0;
}
