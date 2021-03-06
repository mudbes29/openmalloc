#ifndef _openmalloc_h_
#define _openmalloc_h_

#include <string.h>
#include <pthread.h>
/*
 * The strategy (allocation algorithm) to use. Choose from:
 * 1) First fit
 * 2) Best fit
 * 3) Worst fit
 * 4) Quick fit
 * 5) Multithreaded fit
 */
#ifndef STRATEGY
#define STRATEGY 5
#endif

/*
 * The number of lists to use in the Quick fit algorithm implementation.
 */
#ifndef NRQUICKLISTS
#define NRQUICKLISTS 4
#endif

/*
 * The minimum #units to request when asking system for more memory.
 */
#define NALLOC 1024

typedef long Align; /* for alignment to long boundary */

union header {
    /* block header */
    struct {
        union header *ptr;/* next block if on free list */
        unsigned size;
        /* size of this block */
        pthread_mutex_t lock;
    } s;
    Align x;
    /* force alignment of blocks */
};
typedef union header Header;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
static Header *morecore(unsigned nu);

#endif /* _malloc_h */
