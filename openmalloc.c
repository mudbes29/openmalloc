#include "brk.h"
#include "openmalloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * An empty list to get started.
 */
static Header base;

/*
 * Start of the free list, in non-Quick fit strategies.
 */
static Header *freep = NULL;

/* Include the different malloc implementation source files here. */
#include "firstfit.c"
#include "bestfit.c"
#include "worstfit.c"
#include "quickfit.c"
#include "mthreadfit.c"

/* malloc
 *
 * malloc returns the start address of dynamically allocated memory.
 * This is simply a wrapper function that calls the different algorithm
 * implementations depending on the value of STRATEGY.
 *
 */
/* number of bytes of memory to allocate */
void *malloc(size_t nbytes) {
    if (nbytes == 0) {
        return NULL;
    }

#if   STRATEGY == 1
    return malloc_first(nbytes);
#elif STRATEGY == 2
    return malloc_best(nbytes);
#elif STRATEGY == 3
    return malloc_worst(nbytes);
#elif STRATEGY == 4
    return malloc_quick(nbytes);
#elif STRATEGY == 5
    return malloc_mthread(nbytes);
#else
    exit(1);
#endif
}


/* morecore
 *
 * morecore asks system for more memory and returns pointer to it.
 *
 */
/* the amount of memory to ask for (in Header-sized units) */
static Header *morecore(unsigned nu) {
    char *cp;
    Header *up;
    if (nu < NALLOC)
        nu = NALLOC;
    cp = sbrk(nu * sizeof(Header));
    if (cp == (char *) -1) /* no space at all */
        return NULL;
    up = (Header *) cp;
    up->s.size = nu;
    free((void *) (up + 1));
    return freep;
}


/* free
 *
 * Put block ap in free list.
 */
/* pointer to data part of allocated memory block */
void free(void *ap) {
#if STRATEGY == 4
    int list_index;
    Header* old_first_free;
#endif

    if (ap == NULL) {
        return;
    }

    Header *bp, *p;
    bp = (Header *) ap - 1;

    if (bp->s.size <= 0) {
        return;
    }


    /*
     * For the Quick fit strategy, if allocated memory block has been stored in
     * a quick list, simply put the whole block back and return.
     */

#if STRATEGY == 4
    list_index = qindex(sizeof(Header) * (bp->s.size - 1));
    if (list_index < NRQUICKLISTS) {
        old_first_free = quick_fit_lists[list_index];
        quick_fit_lists[list_index] = bp;
        bp->s.ptr = old_first_free;
        return;
    }
#endif


    /*
     * Merge free memory blocks if the one being freed is adjacent to another
     * free one,
     */

    /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break; /* freed block at start or end of arena */


    if (bp + bp->s.size == p->s.ptr) {
        /* join to upper nbr */
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr;

    if (p + p->s.size == bp) {
        /* join to lower nbr */
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else
        p->s.ptr = bp;

    freep = p;
}

/* realloc
 *
 * realloc changes size of the given memory block and returns pointer to new
 */
/* pointer to memory block to resize */
/* the new size, in bytes */
void *realloc(void *ptr, size_t size) {
    Header *bp, *p;

    /*
     * Take care of some simple, basic cases like when the given pointer is
     * NULL or the size is non-positive.
     */

    if (ptr == NULL) {
        return malloc(size);
    } else if (size <= 0) {
        free(ptr);
        return NULL;
    }

    bp = (Header *) ptr - 1;
    unsigned int numbytes = sizeof(Header) * (bp->s.size - 1);

    if (size == numbytes) {
        return ptr; /* stop and return here if no resize is necessary */
    }


    /*
     * Simply allocate as much memory as the new size and copy the old data
     * into the new place. Then free the previously allocated memory.
     */

    p = malloc(size);
    if (p == NULL) {
        return NULL;
    }

    if (size < numbytes) {
        numbytes = size;
    }

    memcpy(p, ptr, numbytes);
    free(ptr);

    return p;
}

