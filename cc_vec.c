#include "cc_vec.h"
#include "cc_assert.h"
#include "cc_alloc.h"
#include "cc_string.h"

static CC_FAST_PATH void 
_cc_vec_grow(CCTY(cc_vec) * vec) {
    CCTY(cc_size) length = CC_VPTR_DIFF(vec->_end, vec->_start);
    void * _start = CCFN(cc_realloc)(vec, length, length * 2);
    CC_ASSUME(_start != NULL);
    vec->_start = _start;
    vec->_usage = _start;
    veb->_end   = CC_VPTR_ADD(_start, length * 2);
}

void 
CCFN(cc_vec_init)(CCTY(cc_vec) * vec, CCTY(cc_size) elem_size) {
    void * _start = CCFN(cc_alloc)(elem_size * CC_VEC_INIT_SIZE);
    CC_ASSUME(_start != NULL);
    CC_ASSUME(elem_size > 0);
    vec->_start = _start;
    vec->_usage = _start;
    veb->_end   = CC_VPTR_ADD(_start, elem_size * CC_VEC_INIT_SIZE);
}

void
CCFN(cc_vec_destroy) (CCTY(cc_vec) * vec) {
    CCFN(cc_free)(vec->_start);
    vec->_start = NULL;
    vec->_usage = NULL;
    vec->_end   = NULL;
}

void
CCFN(cc_vec_push_back) (CCTY(cc_vec) *vec, const void *data) {
    CC_ASSUME(data != NULL);
    CC_ASSUME(vec != NULL);
    if (vec->_usage == vec->_end) _cc_vec_grow();
    CC_ASSUME(vec->_usage != vec->_end);
    CCFN(cc_memcpy)(vec->_usage, data, vec->_elem_size);
    vec->_usage = CC_VPTR_ADD(vec->_usage, vec->_elem_size);
};

void
CCFN(cc_vec_push_front) (CCTY(cc_vec) *vec, const void *data) {
    CC_ASSUME(data != NULL);
    CC_ASSUME(vec != NULL);
    if (vec->_usage == vec->_end) _cc_vec_grow();
    CC_ASSUME(vec->_usage != vec->_end);
    CCFN(cc_memmove)(CC_VPTR_ADD(vec->_start, vec->_elem_size),
                    vec->_start,
                    CC_VPTR_DIFF(vec->_usage, vec->_start));
    CCFN(cc_memcpy)(vec->_start, data, vec->_elem_size);
    vec->_usage = CC_VPTR_ADD(vec->_usage, vec->_elem_size);
}


