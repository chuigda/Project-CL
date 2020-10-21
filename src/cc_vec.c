#include "cc_vec.h"
#include "cc_assert.h"
#include "cc_alloc.h"
#include "cc_string.h"

#if CCLIB_DS_VEC != CC_FEAT_DISABLED

#define CC_UINT8 CCTY(cc_uint8)
#define CC_SIZE CCTY(cc_size)
#define CC_SSIZE CCTY(cc_ssize)
#define CC_VEC CCTY(cc_vec)

#define CC_ALLOC CCFN(cc_alloc)
#define CC_FREE CCFN(cc_free)
#define CC_REALLOC CCFN(cc_realloc)
#define CC_MEMCPY CCFN(cc_memcpy)
#define CC_MEMMOVE CCFN(cc_memmove)

static CC_FAST_PATH void cc_vec_grow(CC_VEC *vec) {
    CC_SIZE length = CC_VPTR_DIFF(vec->end, vec->start);
    CC_SIZE target = (length == 0) ? CC_VEC_INIT_SIZE : length * 2;
    void * start = CC_REALLOC (vec, length, target);
    CC_ASSUME(start != NULL);
    vec->start = start;
    vec->usage = CC_VPTR_ADD(start, length);
    vec->end   = CC_VPTR_ADD(start, target);
}

void
CCFN(cc_vec_shrink) (CC_VEC *vec) {
    CC_SIZE length = CC_VPTR_DIFF(vec->end,   vec->start);
    CC_SIZE size   = CC_VPTR_DIFF(vec->usage, vec->start);
    void * start = CC_REALLOC (vec, length, size);
    CC_ASSUME(start != NULL);
    vec->start = start;
    vec->usage = CC_VPTR_ADD(start, size);
    vec->end   = CC_VPTR_ADD(start, size);
}

void 
CCFN(cc_vec_init) (CC_VEC *vec, CC_SIZE elem_size) {
    CC_ASSUME(elem_size > 0);
    vec->start     = NULL;
    vec->usage     = NULL;
    vec->end       = NULL;
    vec->elem_size = elem_size;
}

void
CCFN(cc_vec_destroy) (CC_VEC *vec) {
    CC_FREE(vec->start);
    vec->start     = NULL;
    vec->usage     = NULL;
    vec->end       = NULL;
    vec->elem_size = 0;
}

void
CCFN(cc_vec_push_back) (CC_VEC *vec, const void *data) {
    CC_ASSUME(data != NULL);
    CC_ASSUME(vec != NULL);
    if (vec->usage == vec->end) cc_vec_grow(vec);
    CC_ASSUME(vec->usage != vec->end);
    CC_MEMCPY(vec->usage, data, vec->elem_size);
    vec->usage = CC_VPTR_ADD(vec->usage, vec->elem_size);
};

void
CCFN(cc_vec_push_front) (CC_VEC *vec, const void *data) {
    CC_ASSUME(data != NULL);
    CC_ASSUME(vec != NULL);
    if (vec->usage == vec->end) cc_vec_grow(vec);
    CC_ASSUME(vec->usage != vec->end);
    CC_MEMMOVE(CC_VPTR_ADD(vec->start, vec->elem_size),
               vec->start,
               CC_VPTR_DIFF(vec->usage, vec->start));
    CC_MEMCPY(vec->start, data, vec->elem_size);
    vec->usage = CC_VPTR_ADD(vec->usage, vec->elem_size);
}

void
CCFN(cc_vec_pop_back) (CC_VEC *vec) {
    CC_ASSUME(vec->usage != vec->start);
    vec->usage = CC_VPTR_SUB(vec->usage, vec->elem_size);
}

void
CCFN(cc_vec_pop_front) (CC_VEC *vec) {
    CC_ASSUME(vec->usage != vec->start);
    CC_MEMMOVE(vec->start,
               CC_VPTR_ADD(vec->start, vec->elem_size),
               CC_VPTR_DIFF(vec->usage, vec->start) - vec->elem_size);
    vec->usage = CC_VPTR_SUB(vec->usage, vec->elem_size);
}

void*
CCFN(cc_vec_front) (const CC_VEC *vec) {
    CC_ASSUME(vec->usage != vec->start);
    return vec->start;
}

void*
CCFN(cc_vec_back) (const CC_VEC *vec) {
    CC_ASSUME(vec->usage != vec->start);
    return CC_VPTR_SUB(vec->usage, vec->elem_size);
}

void*
CCFN(cc_vec_nth) (const CC_VEC *vec, CC_SIZE idx) {
    CC_ASSUME(vec->usage != vec->start);
    CC_ASSUME(CC_VPTR_ADD(vec->start, idx * vec->elem_size)
              <= vec->usage);
    return CC_VPTR_ADD(vec->start, idx * vec->elem_size);
}

void
CCFN(cc_vec_insert) (CC_VEC *vec, CC_SIZE idx, const void *data) {
    CC_ASSUME(data != NULL);
    CC_ASSUME(vec != NULL);
    CC_ASSUME(CC_VPTR_ADD(vec->start, idx * vec->elem_size)
              <= vec->usage);
    if (vec->usage == vec->end) cc_vec_grow(vec);
    void * start_pos = CC_VPTR_ADD(vec->start, idx * vec->elem_size);
    CC_MEMMOVE(CC_VPTR_ADD(start_pos, vec->elem_size),
               start_pos,
               CC_VPTR_DIFF(vec->usage, start_pos));
    CC_MEMCPY(start_pos, data, vec->elem_size);
    vec->usage = CC_VPTR_ADD(vec->usage, vec->elem_size);
}

void
CCFN(cc_vec_remove) (CC_VEC *vec, CC_SIZE idx) {
    CC_ASSUME(vec != NULL);
    CC_ASSUME(CC_VPTR_ADD(vec->start, idx * vec->elem_size)
              <= vec->usage);
    void *start_pos = CC_VPTR_ADD(vec->start, idx * vec->elem_size);
    CC_MEMMOVE(CC_VPTR_SUB(start_pos, vec->elem_size),
               start_pos,
               CC_VPTR_DIFF(vec->usage, start_pos));
    vec->usage = CC_VPTR_SUB(vec->usage, vec->elem_size);
}

void
CCFN(cc_vec_remove_n) (CC_VEC *vec, CC_SIZE first, CC_SIZE n) {
    CC_ASSUME(vec != NULL);
    CC_ASSUME(CC_VPTR_ADD(vec->start, (first + n) * vec->elem_size)
              <= vec->usage);
    if (n == 0) return;
    void *start_pos = CC_VPTR_ADD(vec->start, first * vec->elem_size);
    void *end_pos   = CC_VPTR_ADD(start_pos, n * vec->elem_size);
    CC_MEMMOVE(start_pos,
               end_pos,
               CC_VPTR_DIFF(vec->usage, end_pos));
    vec->usage = CC_VPTR_SUB(vec->usage, n * vec->elem_size);
}

CC_SIZE
CCFN(cc_vec_len) (const CC_VEC *vec) {
    return CC_VPTR_DIFF(vec->end, vec->start) / vec->elem_size;
}

CC_SIZE
CCFN(cc_vec_size) (const CC_VEC *vec) {
    return CC_VPTR_DIFF(vec->usage, vec->start) / vec->elem_size;
}

#endif
