/// @attention: CC_ST_WORD_MASK must be defined
/// @attention: CC_ST_WORD_STRIDE must be defined
/// @attention: cc_st_bitmask_impl must be defined

#include "cc_bits.h"
#include <cc_defs.h>

static inline cc_st_bitmask cc_st_word_mask(void) { return CC_ST_WORD_MASK; }

static inline cc_size cc_st_word_stride(void) { return CC_ST_WORD_STRIDE; }

static inline cc_st_bitmask cc_st_bitmask_invert(cc_st_bitmask bitmask) {
  bitmask ^= cc_st_word_mask();
  return bitmask;
}

static inline _Bool cc_st_bitmask_any(cc_st_bitmask bitmask) {
  return bitmask != 0;
}

static inline cc_size cc_st_bitmask_tzeros(cc_st_bitmask bitmask) {
  return (cc_size)cc_trailing_zeros(bitmask) / cc_st_word_stride();
}

static inline cc_size cc_st_bitmask_lzeros(cc_st_bitmask bitmask) {
  return (cc_size)cc_leading_zeros(bitmask) / cc_st_word_stride();
}

static inline cc_size cc_st_bitmask_lowest_nz(cc_st_bitmask bitmask) {
  CC_ASSUME(bitmask !=
            0); // but tell the compiler not to emit the checking for zero
  return (cc_size)cc_trailing_zeros(bitmask) / cc_st_word_stride();
}

// Some bitmask may not be iterable. We use a struct to make them nominally
// different.
typedef struct {
  cc_st_bitmask mask;
} cc_st_bitmask_iter;

static inline void cc_st_bitmask_iter_rm_lowest(cc_st_bitmask_iter *iter) {
  // Remove the last nonzero bit inside the word:
  //    word              = 011110100 (original value)
  //    word - 1          = 011110011 (invert all bits up to the last set bit)
  //    word & (word - 1) = 011110000 (value with the last bit cleared)
  iter->mask = iter->mask & (iter->mask - 1);
}

static inline cc_size cc_st_bitmask_iter_get_lowest(cc_st_bitmask_iter *iter) {
  return cc_st_bitmask_lowest_nz(iter->mask);
}

static inline _Bool cc_st_bitmask_iter_nonzero(cc_st_bitmask_iter *iter) {
  return iter->mask != 0;
}
