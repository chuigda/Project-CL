#ifndef PROJECT_CL_SWISSTABLE_ASIMD_H
#define PROJECT_CL_SWISSTABLE_ASIMD_H

#include "cc_swisstable/common.h"
#include <cc_defs.h>
#include <wasm_simd128.h>

typedef cc_uint16 cc_st_bitmask;
#define CC_ST_WORD_MASK 0xFFFFU
#define CC_ST_WORD_STRIDE 1

#include "cc_swisstable/bitmask.inc"

typedef v128_t cc_st_group;

static inline cc_st_group cc_st_load_group(const void * __restrict addr)
{
    return wasm_v128_load(addr);
}

static inline cc_st_group
cc_st_load_group_aligned(const void * __restrict addr)
{
    return *(cc_st_group*)(addr);
}

static inline void cc_st_store_group_aligned(void * __restrict addr,
                                             cc_st_group group)
{
    wasm_v128_store(addr, group);
}

static inline cc_st_bitmask_iter
cc_st_group_match_ctrl(cc_st_group group, cc_st_ctrl byte)
{
    cc_st_bitmask_iter iter;
    cc_st_group duplicated = wasm_u8x16_splat(byte);
    cc_st_group cmp = wasm_i8x16_eq(group, duplicated);
    cc_st_bitmask converted = (cc_st_bitmask)wasm_i8x16_bitmask(cmp);
    iter.mask = converted;
    return iter;
}

static inline cc_st_bitmask cc_st_group_mask_empty(cc_st_group group)
{
    return cc_st_group_match_ctrl(group, CC_ST_EMPTY).mask;
}

static inline cc_st_bitmask
cc_st_group_mask_empty_or_deleted(cc_st_group group)
{
    return (cc_st_bitmask)wasm_i8x16_bitmask(group);
}

static inline cc_st_bitmask cc_st_group_mask_full(cc_st_group group)
{
    return cc_st_bitmask_invert(
        cc_st_group_mask_empty_or_deleted(group));
}

static inline cc_st_group
cc_st_group_convert_special_to_empty_and_full_to_deleted(
    cc_st_group group)
{
    cc_st_group zero = wasm_u8x16_splat(0);
    cc_st_group special = wasm_i8x16_gt(zero, group);
    cc_st_group converted
        = wasm_v128_or(special, wasm_u8x16_splat(0x80u));
    return converted;
}

#endif // PROJECT_CL_SWISSTABLE_ASIMD_H
