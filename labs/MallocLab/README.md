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

## Explicit List Allocator

> [Explicit List](https://cs.wellesley.edu/~cs240/s19/slides/malloc.pdf#page=20)

The trade off: 60% + 40%

* 60%: Space utilization (Memory)
* 40%: Throughput (Time)

Fit algorithm:

| Placement Policy | Memory | Time |
|------------------|--------|------|
| First Fit        | $B$    | $B$  |
| Next Fit         | $C$    | $A$  |
| Best Fit         | $A$    | $C$  |

Order strategy:

| Order | Memory | Time   |
|-------|--------|--------|
| LIFO  | $B$    | $O(1)$ |
| Addr  | $A-$   | $O(n)$ |

-- The above table might be wrong, please think over it.

I will use LIFO & First Fit.

```
Allocated:                Free:
┌────────┬─────┐          ┌────────┬─────┐
│  size  │ a/f │          │  size  │ a/f │
├────────┴─────┤          ├────────┴─────┤
│              │          │   prev_link  │ 
│    Payload   │          ├──────────────┤    
│              │          │   next_link  │
├──────────────┤          ├──────────────┤
│    Padding   │          │    Padding   │
├────────┬─────┤          ├────────┬─────┤
│  size  │ a/f │          │  size  │ a/f │
└────────┴─────┘          └────────┴─────┘

Abstract:
   ┌─────┐       ┌─────┐       ┌─────┐
<- │  H  │  <->  │  B  │  <->  │  T  │ ->
   └─────┘       └─────┘       └─────┘
* H: Head
* B: Blocks
* T: Tail
```

### Debug

<strong>*</strong> `mm_malloc()` fails => bug01

```diff
static void *find_fit(size_t asize) {
    char *bp = free_listp;
    /* FIRST-FIT Search */
<   for (; GET_ALLOC(HDRP(bp)) == 0; bp = NEXT_BLKP(bp)) {
>   for (; GET_ALLOC(HDRP(bp)) == 0; bp = NEXT_LINK(bp)) {
        if (asize <= GET_SIZE(HDRP(bp)))
            return bp;
    }
    return NULL; // No fit found
}
```

<strong>*</strong> `mm_check()` header does not match footer => bug02
