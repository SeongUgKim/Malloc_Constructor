# Malloc_Constructor
Implement malloc/free function 

## Learning Object
1. Manipulate C pointers to traverse a process’ address space
2. Use pointer arithmetic to adjust pointer references
3. Use casting to dereference memory storage as different types
4. Manually adjust the process heap

## Description
### my_malloc()
  allocates size bytes of memory
### my_free()
  deallocates memory referenced by ptr, previously allocated by my_malloc()
### coalesce_free_list()
  merges logically adjacent chunks on the free list into single larger chunks
### free_list_begin()
  retrieves the first node of the free list

