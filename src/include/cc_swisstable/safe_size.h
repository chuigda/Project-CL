//
// Created by Yifan Zhu on 3/26/23.
//

#ifndef PROJECT_CL_SWISSTABLE_SAFE_SIZE_H
#define PROJECT_CL_SWISSTABLE_SAFE_SIZE_H

#include <cc_defs.h>

// Memory size overflow protection

typedef cc_isize cc_st_safe_size;

static inline cc_size cc_st_max_safe_size(void) {
  return ((cc_size)((cc_isize)-1) >> 1U);
}

static inline cc_st_safe_size cc_st_safe_size_create(cc_size size) {
  return CC_UNLIKELY(size <= cc_st_max_safe_size()) ? (cc_st_safe_size)size
                                                    : -1;
}

static inline _Bool cc_st_safe_size_is_valid(cc_st_safe_size size) {
  return size >= 0;
}

static inline cc_st_safe_size cc_st_safe_size_add(cc_st_safe_size a,
                                                  cc_st_safe_size b) {
  if (CC_UNLIKELY(a < 0 || b < 0)) {
    return -1;
  }
  // both are positive. if overflow ever happens, it will be negative.
  return (cc_st_safe_size)((cc_size)a + (cc_size)b);
}

static inline cc_st_safe_size cc_st_safe_size_mul(cc_st_safe_size a,
                                                  cc_st_safe_size b) {
  if (CC_UNLIKELY(a < 0 || b < 0)) {
    return -1;
  }
  cc_st_safe_size result;
#if defined(__has_builtin) && __has_builtin(__builtin_mul_overflow)
  if (__builtin_mul_overflow(a, b, &result)) {
    return -1;
  }
#else
  result = a * b;
  if (CC_UNLIKELY(a != 0 && result / a != b)) {
    return -1;
  }
#endif
  return result;
}

#endif // PROJECT_CL_SWISSTABLE_SAFE_SIZE_H
