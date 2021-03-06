/* malloc_worst
 *
 * This module contains the function that implements the Worst fit malloc
 * strategy, malloc_worst().
 *
 * malloc_worst returns the start address of the newly allocated memory.
 * It implements the Worst fit algorithm, which tries to find a free memory
 * block that is as large as possible.
 *
 */
void *malloc_worst(size_t nbytes) { /* number of bytes of memory to allocate */

    Header *p, *prevp;
    Header *moreroce(unsigned);
    unsigned nunits, max_size = 0;
    Header *maxp = NULL, *maxprevp = NULL;

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL) { /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    /*
     * Iterate over the free list and find the largest block, that is large
     * enough to hold nbytes of data.
     */

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) { /* big enough */
            if (p->s.size == nunits) { /* exactly */
                prevp->s.ptr = p->s.ptr;
                freep = prevp;
                return (void *)(p + 1);
            }
            else {
                if (maxp == NULL || p->s.size > max_size) {
                    maxp = p;
                    maxprevp = prevp;
                    max_size = maxp->s.size;
                }
            }
        }
        if (p == freep) { /* wrapped around free list */
            if (maxp != NULL) {
                /* allocate tail end */
                maxp->s.size -= nunits;
                maxp += maxp->s.size;
                maxp->s.size = nunits;
                freep = maxprevp;
                return (void *)(maxp + 1);
            }
            if ((p = morecore(nunits)) == NULL)
                return NULL; /* none left */
        }
    }
}
