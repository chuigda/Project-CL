#ifndef CCLIB_LIST_H
#define CCLIB_LIST_H

#include "cfg.h"

#if CCLIB_DS_LLIST != CC_FEAT_DISABLED

#include "cc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_cc_list {
    void *p1;
    void *p2;
    CCTY(cc_size) elem_size;
} CCTY(cc_list);

typedef struct st_cc_list_iter {
    void *node;
} CCTY(cc_list_iter);

void
CCFN(cc_list_init) (CCTY(cc_list) *list, CCTY(cc_size) elem_size);

void
CCFN(cc_list_destroy) (CCTY(cc_list) *list);

void
CCFN(cc_list_push_back) (CCTY(cc_list) *list, const void *data);

void
CCFN(cc_list_push_front) (CCTY(cc_list) *list, const void *data);

void
CCFN(cc_list_pop_back) (CCTY(cc_list) *list);

void
CCFN(cc_list_pop_front) (CCTY(cc_list) *list);

void*
CCFN(cc_list_front) (const CCTY(cc_list) *list);

void*
CCFN(cc_list_back) (const CCTY(cc_list) *list);

CCTY(cc_size)
CCFN(cc_list_len) (const CCTY(cc_list) *list);

CCTY(cc_size)
CCFN(cc_list_size) (const CCTY(cc_list) *list);

_Bool
CCFN(cc_list_empty) (const CCTY(cc_list) *list);

CCTY(cc_list_iter)
CCFN(cc_list_begin) (const CCTY(cc_list) *list);

CCTY(cc_list_iter)
CCFN(cc_list_end) (const CCTY(cc_list) *list);

void
CCFN(cc_list_insert) (const CCTY(cc_list) *list,
                      CCTY(cc_list_iter) pos,
                      const void *data);

void
CCFN(cc_list_insert_after) (const CCTY(cc_list) *list,
                            CCTY(cc_list_iter) pos,
                            const void *data);

void
CCFN(cc_list_remove) (const CCTY(cc_list) *list,
                      CCTY(cc_list_iter) pos);

void
CCFN(cc_list_remove_range) (const CCTY(cc_list) *list,
                            CCTY(cc_list_iter) first,
                            CCTY(cc_list_iter) last);

void*
CCFN(cc_list_iter_deref) (CCTY(cc_list_iter) iter);

CCTY(cc_list_iter)
CCFN(cc_list_iter_prev) (CCTY(cc_list_iter) iter);

CCTY(cc_list_iter)
CCFN(cc_list_iter_next) (CCTY(cc_list_iter) iter);

_Bool
CCFN(cc_list_iter_eq) (CCTY(cc_list_iter) iter1,
                       CCTY(cc_list_iter) iter2);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif /* CCLIB_LIST_H */
