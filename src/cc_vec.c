#include "cc_vec.h"
#include "cc_assert.h"
#include "cc_alloc.h"
#include "cc_string.h"

#if CCLIB_DS_VEC != CC_FEAT_DISABLED

static CC_FAST_PATH void 
_cc_vec_grow(CCTY(cc_vec) * vec) {
    CCTY(cc_size) length = CC_VPTR_DIFF(vec->_end, vec->_start);
    CCTY(cc_size) target = (length == 0) ? CC_VEC_INIT_SIZE : length * 2;
    void * _start = CCFN(cc_realloc) (vec, length, target);
    CC_ASSUME(_start != NULL);
    vec->_start = _start;
    vec->_usage = CC_VPTR_ADD(_start, length);
    vec->_end   = CC_VPTR_ADD(_start, target);
}

void
CCFN(cc_vec_shrink) (CCTY(cc_vec) *vec) {
    CCTY(cc_size) length = CC_VPTR_DIFF(vec->_end,   vec->_start);
    CCTY(cc_size) size   = CC_VPTR_DIFF(vec->_usage, vec->_start);
    void * _start = CCFN(cc_realloc) (vec, length, size);
    CC_ASSUME(_start != NULL);
    vec->_start = _start;
    vec->_usage = CC_VPTR_ADD(_start, size);
    vec->_end   = CC_VPTR_ADD(_start, size);
}

void 
CCFN(cc_vec_init) (CCTY(cc_vec) * vec, CCTY(cc_size) elem_size) {
    CC_ASSUME(elem_size > 0);
    vec->_start     = NULL;
    vec->_usage     = NULL;
    vec->_end       = NULL;
    vec->_elem_size = elem_size;
}

void
CCFN(cc_vec_destroy) (CCTY(cc_vec) * vec) {
    CCFN(cc_free) (vec->_start);
    vec->_start     = NULL;
    vec->_usage     = NULL;
    vec->_end       = NULL;
    vec->_elem_size = 0;
}

void
CCFN(cc_vec_push_back) (CCTY(cc_vec) *vec, const void *data) {
    CC_ASSUME(data != NULL);
    CC_ASSUME(vec != NULL);
    if (vec->_usage == vec->_end) _cc_vec_grow(vec);
    CC_ASSUME(vec->_usage != vec->_end);
    CCFN(cc_memcpy) (vec->_usage, data, vec->_elem_size);
    vec->_usage = CC_VPTR_ADD(vec->_usage, vec->_elem_size);
};

void
CCFN(cc_vec_push_front) (CCTY(cc_vec) *vec, const void *data) {
    CC_ASSUME(data != NULL);
    CC_ASSUME(vec != NULL);
    if (vec->_usage == vec->_end) _cc_vec_grow(vec);
    CC_ASSUME(vec->_usage != vec->_end);
    CCFN(cc_memmove) (CC_VPTR_ADD(vec->_start, vec->_elem_size),
                      vec->_start,
                      CC_VPTR_DIFF(vec->_usage, vec->_start));
    CCFN(cc_memcpy) (vec->_start, data, vec->_elem_size);
    vec->_usage = CC_VPTR_ADD(vec->_usage, vec->_elem_size);
}

void
CCFN(cc_vec_pop_back) (CCTY(cc_vec) *vec) {
    CC_ASSUME(vec->_usage != vec->_start);
    vec->_usage = CC_VPTR_SUB(vec->_usage, vec->_elem_size);
}

void
CCFN(cc_vec_pop_front) (CCTY(cc_vec) *vec) {
    CC_ASSUME(vec->_usage != vec->_start);
    CCFN(cc_memmove) (vec->_start,
                      CC_VPTR_ADD(vec->_start, vec->_elem_size),
                      CC_VPTR_DIFF(vec->_usage, vec->_start) 
                      - vec->_elem_size);
    vec->_usage = CC_VPTR_SUB(vec->_usage, vec->_elem_size);
}

void*
CCFN(cc_vec_front) (const CCTY(cc_vec) *vec) {
    CC_ASSUME(vec->_usage != vec->_start);
    return vec->_start;
}

void*
CCFN(cc_vec_back) (const CCTY(cc_vec) *vec) {
    CC_ASSUME(vec->_usage != vec->_start);
    return CC_VPTR_SUB(vec->_usage, vec->_elem_size);
}

void*
CCFN(cc_vec_nth) (const CCTY(cc_vec) *vec, CCTY(cc_size) idx) {
    CC_ASSUME(vec->_usage != vec->_start);
    CC_ASSUME(CC_VPTR_ADD(vec->_start, idx * vec->_elem_size)
              <= vec->_usage);
    return CC_VPTR_ADD(vec->_start, idx * vec->_elem_size);
}

void
CCFN(cc_vec_insert) (CCTY(cc_vec) *vec,
                     CCTY(cc_size) idx,
                     const void *data) {
    CC_ASSUME(data != NULL);
    CC_ASSUME(vec != NULL);
    CC_ASSUME(CC_VPTR_ADD(vec->_start, idx * vec->_elem_size)
              <= vec->_usage);
    if (vec->_usage == vec->_end) _cc_vec_grow(vec);
    void * start_pos = CC_VPTR_ADD(vec->_start, idx * vec->_elem_size);
    CCFN(cc_memmove) (CC_VPTR_ADD(start_pos, vec->_elem_size),
                      start_pos,
                      CC_VPTR_DIFF(vec->_usage, start_pos));
    CCFN(cc_memcpy) (start_pos, data, vec->_elem_size);
    vec->_usage = CC_VPTR_ADD(vec->_usage, vec->_elem_size);
}

void
CCFN(cc_vec_remove) (CCTY(cc_vec) *vec, CCTY(cc_size) idx) {
    CC_ASSUME(vec != NULL);
    CC_ASSUME(CC_VPTR_ADD(vec->_start, idx * vec->_elem_size)
              <= vec->_usage);
    void *start_pos = CC_VPTR_ADD(vec->_start, idx * vec->_elem_size);
    CCFN(cc_memmove) (CC_VPTR_SUB(start_pos, vec->_elem_size),
                      start_pos,
                      CC_VPTR_DIFF(vec->_usage, start_pos));
    vec->_usage = CC_VPTR_SUB(vec->_usage, vec->_elem_size);
}

void
CCFN(cc_vec_remove_n) (CCTY(cc_vec) *vec,
                       CCTY(cc_size) first,
                       CCTY(cc_size) n) {
    CC_ASSUME(vec != NULL);
    CC_ASSUME(CC_VPTR_ADD(vec->_start, (first + n) * vec->_elem_size)
              <= vec->_usage);
    if (n == 0) return;
    void *start_pos = CC_VPTR_ADD(vec->_start, first * vec->_elem_size);
    void *end_pos   = CC_VPTR_ADD(start_pos, n * vec->_elem_size);
    CCFN(cc_memmove) (start_pos,
                      end_pos,
                      CC_VPTR_DIFF(vec->_usage, end_pos));
    vec->_usage = CC_VPTR_SUB(vec->_usage, n * vec->_elem_size);
}

CCTY(cc_size)
CCFN(cc_vec_len) (const CCTY(cc_vec) *vec) {
    return CC_VPTR_DIFF(vec->_end, vec->_start) / vec->_elem_size;
}

CCTY(cc_size)
CCFN(cc_vec_size) (const CCTY(cc_vec) *vec) {
    return CC_VPTR_DIFF(vec->_usage, vec->_start) / vec->_elem_size;
}

#endif
