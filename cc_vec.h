#ifndef CCLIB_VEC_H
#define CCLIB_VEC_H

#include "cfg.h"

#if CCLIB_DS_VEC != CC_FEAT_DISABLED

#include "cc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_cc_vec {
    /* TODO unimplemented */
} CCTY(cc_vec);

void
CCFN(cc_vec_init) (CCTY(cc_vec) *vec, CCTY(cc_size) elem_size);

void
CCFN(cc_vec_destroy) (CCTY(cc_vec) *vec);

void
CCFN(cc_vec_push_back) (CCTY(cc_vec) *vec, const void *data);

void
CCFN(cc_vec_push_front) (CCTY(cc_vec) *vec, const void *data);

void*
CCFN(cc_vec_pop_back) (CCTY(cc_vec) *vec);

void*
CCFN(cc_vec_pop_front) (CCTY(cc_vec) *vec);

void*
CCFN(cc_vec_front) (const CCTY(cc_vec) *vec);

void*
CCFN(cc_vec_back) (const CCTY(cc_vec) *vec);

void*
CCFN(cc_vec_nth) (const CCTY(cc_vec) *vec, CCTY(cc_size) idx);

void
CCFN(cc_vec_insert) (CCTY(cc_vec) *vec,
                     CCTY(cc_size) idx,
                     const void *data);

void
CCFN(cc_vec_remove) (CCTY(cc_vec) *vec, CCTY(cc_size) idx);

void
CCFN(cc_vec_remove_n) (CCTY(cc_vec) *vec,
                       CCTY(cc_size) first,
                       CCTY(cc_size) n);

CCTY(cc_size)
CCFN(cc_vec_len) (const CCTY(cc_vec) *vec);

CCTY(cc_size)
CCFN(cc_vec_size) (const CCTY(cc_vec) *vec);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif /* CCLIB_VEC_H */
