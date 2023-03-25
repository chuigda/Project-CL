/// @note: CC_ST_WORD_MASK must be defined
/// @note: CC_ST_WORD_STRIDE must be defined
/// @note: cc_st_bitmask_impl must be defined

#include <cc_assert.h>
#include <cc_defs.h>
#include "cc_bits.h"

static inline
cc_st_bitmask_impl cc_st_word_mask(void) {
    return CC_ST_WORD_MASK;
}

static inline
cc_size cc_st_word_stride(void) {
    return CC_ST_WORD_STRIDE;
}

typedef struct {
    cc_st_bitmask_impl word;
} cc_st_bitmask;

static inline
cc_st_bitmask cc_st_bitmask_invert(cc_st_bitmask bitmask) {
    bitmask.word ^= cc_st_word_mask();
    return bitmask;
}

static inline
cc_st_bitmask cc_st_bitmask_xor(cc_st_bitmask bitmask, cc_st_bitmask_impl word) {
    bitmask.word ^= word;
    return bitmask;
}

static inline
_Bool cc_st_bitmask_any(cc_st_bitmask bitmask) {
    return bitmask.word != 0;
}

static inline
cc_size cc_st_bitmask_tzeros(cc_st_bitmask bitmask) {
    return (cc_size) cc_trailing_zeros(bitmask.word)
           / cc_st_word_stride();
}

static inline
cc_size cc_st_bitmask_lzeros(cc_st_bitmask bitmask) {
    return (cc_size) cc_leading_zeros(bitmask.word)
           / cc_st_word_stride();
}

static inline
cc_size cc_st_bitmask_lowest_nz(cc_st_bitmask bitmask) {
    CC_ASSUME(bitmask.word != 0); // tell the compiler not to emit the checking for zero
    cc_assert(bitmask.word != 0); // but do check the correctness for debug build
    return (cc_size) cc_trailing_zeros(bitmask.word)
           / cc_st_word_stride();
}

typedef struct {
    cc_st_bitmask mask;
} cc_st_bitmask_iter;

static inline
void cc_st_bitmask_iter_rm_lowest(cc_st_bitmask_iter *iter) {
    // Remove the last nonzero bit inside the word:
    //    word              = 011110100 (original value)
    //    word - 1          = 011110011 (invert all bits up to the last set bit)
    //    word & (word - 1) = 011110000 (value with the last bit cleared)
    iter->mask.word = iter->mask.word & (iter->mask.word - 1);
}

static inline
cc_size cc_st_bitmask_iter_get_lowest(cc_st_bitmask_iter *iter) {
    return cc_st_bitmask_lowest_nz(iter->mask);
}

static inline
_Bool cc_st_bitmask_iter_nonzero(cc_st_bitmask_iter *iter) {
    return iter->mask.word != 0;
}



