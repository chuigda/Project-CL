#ifndef CCLIB_ALLOC_H
#define CCLIB_ALLOC_H

#include "cfg.h"

#if CCLIB_ALLOC != CC_FEAT_DISABLED

#include "cc_defs.h"

void* CCFN(cc_alloc) (CCTY(cc_size) size);
void* CCFN(cc_realloc) (void *ptr,
                        CCTY(cc_size) old_size,
                        CCTY(cc_size) new_size);
void CCFN(cc_free) (void *ptr);

#endif
#endif /* CCLIB_ALLOC_H */
