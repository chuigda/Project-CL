#ifndef PROJECT_CL_SWISSTABLE_ASIMD_H
#define PROJECT_CL_SWISSTABLE_ASIMD_H

#include <cc_defs.h>
#include <immintrin.h>
#include "cc_swisstable/common.h"

typedef cc_uint16 cc_st_bitmask;
#define CC_ST_WORD_MASK   0xFFFFU
#define CC_ST_WORD_STRIDE 1

#include "cc_swisstable/bitmask.inc"

typedef __m128i cc_st_group;

static inline
cc_st_group cc_st_load_group(const void *__restrict addr) {
    return _mm_loadu_si128((const __m128i *) addr);
}

static inline
cc_st_group cc_st_load_group_aligned(const void *__restrict addr) {
    return _mm_load_si128((const __m128i *) addr);
}

static inline void
cc_st_store_group_aligned(void *__restrict addr, cc_st_group group) {
    _mm_store_si128((__m128i *) addr, group);
}

static inline
cc_st_bitmask_iter cc_st_group_match_ctrl(cc_st_group group, cc_st_ctrl byte) {
    cc_st_bitmask_iter iter;
    cc_st_group duplicated = _mm_set1_epi8((int8_t) byte);
    cc_st_group cmp = _mm_cmpeq_epi8(group, duplicated);
    cc_st_bitmask converted = (cc_st_bitmask) _mm_movemask_epi8(cmp);
    iter.mask = converted;
    return iter;
}

static inline
cc_st_bitmask cc_st_group_mask_empty(cc_st_group group) {
    return cc_st_group_match_ctrl(group, CC_ST_EMPTY).mask;
}

static inline
cc_st_bitmask cc_st_group_mask_empty_or_deleted(cc_st_group group) {
    return (cc_st_bitmask) _mm_movemask_epi8(group);
}

static inline
cc_st_bitmask cc_st_group_mask_full(cc_st_group group) {
    return cc_st_bitmask_invert(
            cc_st_group_mask_empty_or_deleted(group));
}

static inline cc_st_group
cc_st_group_convert_special_to_empty_and_full_to_deleted(
        cc_st_group group) {
    cc_st_group zero = _mm_setzero_si128();
    cc_st_group special = _mm_cmpgt_epi8(zero, group);
    cc_st_group converted = _mm_or_si128(special, _mm_set1_epi8(0x80u));
    return converted;
}

#endif // PROJECT_CL_SWISSTABLE_ASIMD_H
