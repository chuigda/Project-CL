#include "cc_alloc.h"
#include <stdlib.h>

void* CCFN(cc_alloc) (CCTY(cc_size) size) {
    return malloc((size_t)size);
}

void* CCFN(cc_realloc) (void *ptr,
                        CCTY(cc_size) old_size,
                        CCTY(cc_size) new_size) {
    (void)old_size;
    return realloc(ptr, (size_t)new_size);
}

void CCFN(cc_free) (void *ptr) {
    free(ptr);
}
