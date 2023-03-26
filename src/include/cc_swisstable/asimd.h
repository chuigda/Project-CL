#ifndef PROJECT_CL_SWISSTABLE_ASIMD_H
#define PROJECT_CL_SWISSTABLE_ASIMD_H

#include "cc_swisstable/common.h"
#include <arm_neon.h>
#include <cc_defs.h>

typedef cc_uint64 cc_st_bitmask;
#define CC_ST_WORD_MASK 0x8080808080808080ULL
#define CC_ST_WORD_STRIDE 8

#include "cc_swisstable/bitmask.inc"

typedef int8x8_t cc_st_group;

static inline cc_st_group cc_st_load_group(const void *__restrict addr) {
  return vld1_s8((const int8_t *)addr);
}

static inline cc_st_group
cc_st_load_group_aligned(const void *__restrict addr) {
  return vld1_s8((const int8_t *)addr);
}

static inline void cc_st_store_group_aligned(void *__restrict addr,
                                             cc_st_group group) {
  vst1_s8((int8_t *)addr, group);
}

static inline cc_st_bitmask_iter cc_st_group_match_ctrl(cc_st_group group,
                                                        cc_st_ctrl byte) {
  cc_st_bitmask_iter iter;
  cc_st_group duplicated = vdup_n_s8((int8_t)byte);
  cc_st_group cmp = vceq_s8(group, duplicated);
  cc_st_bitmask converted = vget_lane_u64(vreinterpret_u64_u8(cmp), 0);
  iter.mask = converted;
  return iter;
}

static inline cc_st_bitmask cc_st_group_mask_empty(cc_st_group group) {
  cc_st_group duplicated = vdup_n_s8(CC_ST_EMPTY);
  cc_st_group cmp = vceq_s8(group, duplicated);
  cc_st_bitmask converted = vget_lane_u64(vreinterpret_u64_u8(cmp), 0);
  return converted;
}

static inline cc_st_bitmask
cc_st_group_mask_empty_or_deleted(cc_st_group group) {
  cc_st_bitmask converted = vget_lane_u64(vreinterpret_u64_s8(group), 0);
  return converted & cc_st_word_mask();
}

static inline cc_st_bitmask cc_st_group_mask_full(cc_st_group group) {
  return cc_st_bitmask_invert(cc_st_group_mask_empty_or_deleted(group));
}

static inline cc_st_group
cc_st_group_convert_special_to_empty_and_full_to_deleted(cc_st_group group) {
  cc_st_group dup = vdup_n_s8((int8_t)0x80);
  cc_st_group zero = vdup_n_s8((int8_t)0x00);
  cc_st_group special = vcgt_s8(zero, group);
  return vorr_s8(dup, vreinterpret_s8_u8(special));
}

#endif // PROJECT_CL_SWISSTABLE_ASIMD_H
