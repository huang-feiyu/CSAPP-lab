# Malloc Lab

This lab gives students a clear understanding of data layout and organization,
and requires them to evaluate different trade-offs between space and time efficiency.

[WriteUp](http://csapp.cs.cmu.edu/3e/malloclab.pdf)

## Prepare

What we are going to implement:
* `int mm_init(void)`: Initialize the malloc package
* `void *mm_malloc(size_t size)`: Allocate a block
* `void mm_free(void *ptr)`: Free a block
* `void *mm_realloc(void *ptr, size_t size)`: Change the size of the memory
  block pointed to by ptr to size bytes
* `int mm_check(void)`: `checkRep()` for the heap

Helper functions:
* `void *mem sbrk(int incr)`: Expands the heap by incr bytes, where incr is a
  positive non-zero integer and returns a generic pointer to the first byte of
  the newly allocated heap area. The semantics are identical to the Unix sbrk
  function, except that mem sbrk accepts only a positive non-zero integer argument.
* `void *mem heap lo(void)`: Returns a generic pointer to the first byte in the heap.
* `void *mem heap hi(void)`: Returns a generic pointer to the last byte in the heap.
* `size_t mem heapsize(void)`: Returns the current size of the heap in bytes.
* `size_t mem pagesize(void)`: Returns the system’s page size in bytes (4K on Linux systems).

## Implicit List Allocator

In this version of implementation, I will use the textbook implementation to
fully understand the allocator. For further details, please refer to the textbook.

| Placement Policy | Score |
|------------------|-------|
| First Fit        | 63    |
| Next Fit         | 84    |
