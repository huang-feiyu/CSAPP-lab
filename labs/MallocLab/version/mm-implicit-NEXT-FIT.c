/*
 * mm-implicit.c - The implicit free list implementation of malloc/free.
 *
 * Textbook version of implicit free list, NEXT FIT.
 */
#include "mm.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
        /* Team name */
        "HuangBlog",
        /* First member's full name */
        "Huang",
        /* First member's email address */
        "0711feiyu@gmail.com",
        /* Second member's full name (leave blank if none) */
        "",
        /* Second member's email address (leave blank if none) */
        ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// =========================MACROS=============================================
/* Basic constants and macros */
#define WSIZE 4           /* Word and header/footer size (bytes) */
#define DSIZE 8           /* Double word size (bytes) */
#define CHUNKSIZE (1<<12) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc)) // 32:{blockSize[31:3], afFlag[2:0]]}

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7) // size of the address
#define GET_ALLOC(p) (GET(p) & 0x1) // is allocated?

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)                      // header of the address
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // footer of the address

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) // previous block of the address
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) // next block of the address

// ========================Global Variables===================================
static char *heap_listp = 0;  // Pointer to first block

static char *rover = 0; // Next fit rover

// ============Function prototypes for internal helper routines================
int mm_check(void);
static void *extend_heap(size_t);
static void place(void *, size_t);
static void *find_fit(size_t);
static void *coalesce(void *);
static void printblock(void *);
static void checkheap(int);
static void checkblock(void *);

/*
 * mm_init - Initialize the memory manager.
 */
int mm_init(void) {
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *) -1)
        return -1;
    PUT(heap_listp, 0);                            // alignment padding; undefined
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); // prologue header; 8/1
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); // prologue footer; 8/1
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));     // epilogue header; 0/1

    heap_listp += (2 * WSIZE); // now the pointer is between prologue header and footer

    rover = heap_listp;

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *
 * The minimum size of the block is 16 bytes:
 *  - 8 bytes for the header and footer,
 *  - 8 bytes for the alignment padding.
 *
 * For size > 8, the size [+overhead] is rounded up to the next multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    if (heap_listp == 0) {
        mm_init();
    }
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    size_t asize;      // adjusted block size
    size_t extendsize; // amount to extend heap if no fit is found
    char *bp;          // block pointer

    /* Adjust block size to include overhead and alignment reqs */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Free a block
 */
void mm_free(void *bp) {
    if (heap_listp == NULL)
        mm_init();
    if (bp == NULL)
        return;

    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * mm_realloc - Naive implementation of realloc
 */
void *mm_realloc(void *ptr, size_t size) {
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
        return mm_malloc(size);

    newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if (!newptr)
        return 0;

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(ptr));
    if (size < oldsize)
        oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* Free the old block. */
    mm_free(ptr);

    return newptr;
}

/**
 * mm_check - Check the consistency of the memory heap
 */
int mm_check(void) {
    checkheap(1);
    return 0;
}

/**
 * extend_heap - Extend the heap with a free block and return its block pointer
 *
 * Call scene:
 *   1. Heap initialization
 *   2. When mm_malloc cannot find a proper block to allocate
 */
static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long) (bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         // Free block header
    PUT(FTRP(bp), PACK(size, 0));         // Free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // New epilogue header

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/**
 * place - Place the block of aszie bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));
    if ((csize - asize) >= (2 * DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    } else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

/**
 * find_fit - Find a fit for a block with asize bytes
 */
static void *find_fit(size_t asize) {
    char *oldrover = rover;

    /* Search the free list for a fit */
    for (; GET_SIZE(HDRP(rover)) > 0; rover = NEXT_BLKP(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    /* Search from start of the list to old rover */
    for (rover = heap_listp; rover < oldrover; rover = NEXT_BLKP(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    return NULL; // No fit found
}

/**
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 *
 * CS:APP figure 9-40
 */
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            // Case 1
        // No need to coalesce
        return bp;

    } else if (prev_alloc && !next_alloc) {    // Case 2
        // Remove next block from the free list and combine it with current block
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

    } else if (!prev_alloc && next_alloc) {    // Case 3
        // Remove previous block from the free list and combine it with current block
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);

    } else {                                  // Case 4
        // Remove previous block from the free list and combine it with next block
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    /* Make sure the rover isn't pointing into the free block that we just coalesced */
    if ((rover > (char *) bp) && (rover < NEXT_BLKP(bp)))
        rover = bp;

    return bp;
}

/**
 * printblock - Prints the block
 */
static void printblock(void *bp) {
    size_t hsize, halloc, fsize, falloc;

    checkheap(0);
    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));

    if (hsize == 0) {
        printf("%p: EOL\n", bp);
        return;
    }

//     printf("%p: header: [%zu:%c] footer: [%zu:%c]\n", bp, hsize, (halloc ? 'a' : 'f'), fsize, (falloc ? 'a' : 'f'));
}

/**
 * checkblock - Check if the block is valid
 */
static void checkblock(void *bp) {
    if ((size_t) bp % 8)
        printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
        printf("Error: header does not match footer\n");
}

/**
 * checkheap - Check the heap for consistency
 */
void checkheap(int verbose) {
    char *bp = heap_listp;

    if (verbose)
        printf("Heap (%p):\n", heap_listp);

    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
        printf("Bad prologue header\n");
    checkblock(heap_listp);

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (verbose)
            printblock(bp);
        checkblock(bp);
    }

    if (verbose)
        printblock(bp);
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
        printf("Bad epilogue header\n");
}
