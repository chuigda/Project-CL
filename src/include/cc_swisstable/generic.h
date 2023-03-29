#ifndef PROJECT_CL_SWISSTABLE_ASIMD_H
#define PROJECT_CL_SWISSTABLE_ASIMD_H

#include "cc_bits.h"
#include "cc_swisstable/common.h"
#include <cc_defs.h>

typedef cc_size cc_st_bitmask;
typedef cc_size cc_st_group;

// Helper function to spread a byte across the whole word.
// Accumutively, the procedure looks like:
//    byte                  = 0x00000000000000ff
//    byte | (byte << 8)    = 0x000000000000ffff
//    byte | (byte << 16)   = 0x00000000ffffffff
//    byte | (byte << 32)   = 0xffffffffffffffff
static inline cc_size cc_st_repeat(cc_size byte)
{
    cc_size shift_amount = 8;
    while (shift_amount < sizeof(cc_size) * 8)
    {
        byte |= byte << shift_amount;
        shift_amount <<= 1;
    }
    return byte;
}

#define CC_ST_WORD_MASK (cc_st_repeat(0x80U))
#define CC_ST_WORD_STRIDE 8U

#include "cc_swisstable/bitmask.inc"

static inline cc_st_group cc_st_load_group(const void * __restrict addr)
{
    cc_st_group group;
#if defined(__has_builtin) && __has_builtin(__builtin_memcpy_inline)
    __builtin_memcpy_inline(&group, addr, sizeof(cc_st_group));
#elif defined(__has_builtin) && __has_builtin(__builtin_memcpy)
    __builtin_memcpy(&group, addr, sizeof(cc_st_group));
#else
    for (cc_size i = 0; i < sizeof(cc_st_group); i++)
    {
        ((cc_uint8 *)&group)[i] = ((cc_uint8 *)addr)[i];
    }
#endif
    return group;
}

static inline cc_st_group
cc_st_load_group_aligned(const void * __restrict addr)
{
#if defined(__has_builtin) && __has_builtin(__builtin_assume_aligned)
    addr = __builtin_assume_aligned(addr, sizeof(cc_st_group));
#endif
    return *(cc_st_group *)addr;
}

static inline void cc_st_store_group_aligned(void * __restrict addr,
                                             cc_st_group group)
{
#if defined(__has_builtin) && __has_builtin(__builtin_assume_aligned)
    addr = __builtin_assume_aligned(addr, sizeof(cc_st_group));
#endif
    *(cc_st_group *)addr = group;
}

static inline cc_st_group cc_st_as_little_endian(cc_st_group group)
{
    union
    {
        cc_st_group group;
        cc_uint8 bytes[sizeof(cc_st_group)];
    } group_union;
    group_union.group = group;
    if (PROJECT_CL_IS_BIG_ENDIAN())
    {
        for (cc_size i = 0; i < sizeof(cc_st_group) / 2; i++)
        {
            cc_uint8 tmp = group_union.bytes[i];
            group_union.bytes[i]
                = group_union.bytes[sizeof(cc_st_group) - i - 1];
            group_union.bytes[sizeof(cc_st_group) - i - 1] = tmp;
        }
    }
    return group_union.group;
}

static inline cc_st_bitmask_iter
cc_st_group_match_ctrl(cc_st_group group, cc_st_ctrl byte)
{
    // Given byte = 0x10, suppose the data is:
    //
    //       data = [ 0x10 | 0x10 | 0x00 | 0xF1 | ... ]
    //
    // First, we compare the byte using XOR operation:
    //
    //        [ 0x10 | 0x10 | 0x10 | 0x10 | ... ]   (0)
    //      ^ [ 0x10 | 0x10 | 0x00 | 0xF1 | ... ]   (1)
    //      = [ 0x00 | 0x00 | 0x10 | 0xE1 | ... ]   (2)
    //
    // Notice that the equal positions will now be 0x00, so if we 
    // substract 0x01 respective to every byte, it will need to carry the
    // substraction to upper bits (assume no carry from the hidden parts)
    //        [ 0x00 | 0x00 | 0x10 | 0xE1 | ... ]   (2)
    //      - [ 0x01 | 0x01 | 0x01 | 0x01 | ... ]   (3)
    //      = [ 0xFE | 0xFF | 0x0F | 0xE0 | ... ]   (4)
    //
    // But there may be some bytes whose highest bit is already set after
    // the xor operation. To rule out these positions, we AND them with 
    // the NOT of the XOR result:
    //
    //        [ 0xFF | 0xFF | 0xEF | 0x1E | ... ]   (5, NOT (2))
    //      & [ 0xFE | 0xFF | 0x0F | 0xE0 | ... ]   (4)
    //      = [ 0xFE | 0xFF | 0x0F | 0x10 | ... ]   (6)
    //
    // To make the bitmask iteratable, only one bit can be set in each 
    // stride. So we AND each byte with 0x80 and keep only the highest 
    // bit:
    //
    //        [ 0xFE | 0xFF | 0x0F | 0x10 | ... ]   (6)
    //      & [ 0x80 | 0x80 | 0x80 | 0x80 | ... ]   (7)
    //      = [ 0x80 | 0x80 | 0x00 | 0x00 | ... ]   (8)
    //
    // However, there are possitbilites for false positives. For example,
    // if the data is [ 0x10 | 0x11 | 0x10 | 0xF1 | ... ]. This only 
    // happens when there is a key only differs from the searched by the 
    // lowest bit. The claims are:
    //
    //  - This never happens for `EMPTY` and `DELETED`, only full 
    //    entries.
    //  - The check for key equality will catch these.
    //  - This only happens if there is at least 1 true match.
    //  - The chance of this happening is very low 
    //    (< 1% chance per byte).
    cc_st_bitmask_iter iter;
    cc_st_bitmask cmp = group ^ cc_st_repeat(byte);
    cc_st_bitmask result = cc_st_as_little_endian(
        (cmp - cc_st_repeat(0x01U)) & ~cmp & cc_st_repeat(0x80U));
    iter.mask = result;
    return iter;
}

static inline cc_st_bitmask cc_st_group_mask_empty(cc_st_group group)
{
    return cc_st_as_little_endian(group & (group << 1)
                                  & cc_st_repeat(0x80U));
}

static inline cc_st_bitmask
cc_st_group_mask_empty_or_deleted(cc_st_group group)
{
    return cc_st_as_little_endian(group) & cc_st_repeat(0x80U);
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
    // Set the highest bit only for positions whose highest bit is not set
    // before.
    //
    //   data = [ 00000000 | 11111111 | 10000000 | ... ]
    //  ~data = [ 11111111 | 00000000 | 00000000 | ... ]
    //   full = [ 10000000 | 00000000 | 00000000 | ... ]

    cc_st_group full = (~group) & cc_st_repeat(0x80U);

    // Inverse the bit and convert `01111111` to `1000000` by
    // add `1` in that bit. The carry will not propogate outside
    // that byte:
    //      ~full = [ 01111111 | 11111111 | 11111111 | ... ]
    //  full >> 7 = [ 00000001 | 00000000 | 00000000 | ... ]
    //     result = [ 10000000 | 11111111 | 11111111 | ... ]
    return ~full + (full >> 7);
}

#endif // PROJECT_CL_SWISSTABLE_ASIMD_H
