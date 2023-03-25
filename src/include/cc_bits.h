#ifndef PROJECT_CL_BITS_H
#define PROJECT_CL_BITS_H

#include <cc_defs.h>

#if defined(__has_builtin) && __has_builtin(__builtin_clz) &&                  \
    __has_builtin(__builtin_clzl) && __has_builtin(__builtin_clzll)
#define CC_LEADING_ZEROS_IMPL(TARGET_TYPE, BASE_TYPE, BUILTIN)                 \
  static inline unsigned int cc_leading_zeros_##TARGET_TYPE(TARGET_TYPE x) {   \
    _Static_assert(sizeof(BASE_TYPE) >= sizeof(TARGET_TYPE),                   \
                   #BASE_TYPE "must be wider than " #BASE_TYPE);               \
    return BUILTIN((BASE_TYPE)x) -                                             \
           (sizeof(BASE_TYPE) - sizeof(TARGET_TYPE)) * 8;                      \
  }
#else
#define CC_LEADING_ZEROS_IMPL(TARGET_TYPE, BASE_TYPE, BUILTIN)                 \
  static inline unsigned int cc_leading_zeros_##TARGET_TYPE(TARGET_TYPE x) {   \
    _Static_assert(sizeof(BASE_TYPE) >= sizeof(TARGET_TYPE), #BASE_TYPE        \
                   " must be wider than or of equal size with " #BASE_TYPE);   \
    BASE_TYPE first_non_zero = 8 * sizeof(TARGET_TYPE);                        \
    while (first_non_zero != 0) {                                              \
      BASE_TYPE mask = 1u << (first_non_zero - 1);                             \
      if (((BASE_TYPE)x) & mask)                                               \
        break;                                                                 \
      first_non_zero--;                                                        \
    }                                                                          \
    return 8 * sizeof(TARGET_TYPE) - first_non_zero;                           \
  }
#endif

CC_LEADING_ZEROS_IMPL(cc_uint8, unsigned int, __builtin_clz)

CC_LEADING_ZEROS_IMPL(cc_uint16, unsigned int, __builtin_clz)

CC_LEADING_ZEROS_IMPL(cc_uint32, unsigned long, __builtin_clzl)

CC_LEADING_ZEROS_IMPL(cc_uint64, unsigned long long, __builtin_clzll)

#undef CC_LEADING_ZEROS_IMPL

#if defined(__has_builtin) && __has_builtin(__builtin_ctz) &&                  \
    __has_builtin(__builtin_ctzl) && __has_builtin(__builtin_ctzll)
#define CC_TRAILING_ZEROS_IMPL(TARGET_TYPE, BASE_TYPE, BUILTIN)                \
  static inline unsigned int cc_trailing_zeros_##TARGET_TYPE(TARGET_TYPE x) {  \
    _Static_assert(sizeof(BASE_TYPE) >= sizeof(TARGET_TYPE), #BASE_TYPE        \
                   " must be wider than or of equal size with " #BASE_TYPE);   \
    return BUILTIN((BASE_TYPE)x);                                              \
  }
#else
#define CC_TRAILING_ZEROS_IMPL(TARGET_TYPE, BASE_TYPE, BUILTIN)                \
  static inline unsigned int cc_trailing_zeros_##TARGET_TYPE(TARGET_TYPE x) {  \
    unsigned int trailing_zeros = 0;                                           \
    if (x == 0)                                                                \
      return 8 * sizeof(TARGET_TYPE);                                          \
    while ((x & 1u) == 0) {                                                    \
      trailing_zeros++;                                                        \
      x >>= 1;                                                                 \
    }                                                                          \
    return trailing_zeros;                                                     \
  }
#endif

CC_TRAILING_ZEROS_IMPL(cc_uint8, unsigned int, __builtin_ctz)

CC_TRAILING_ZEROS_IMPL(cc_uint16, unsigned int, __builtin_ctz)

CC_TRAILING_ZEROS_IMPL(cc_uint32, unsigned long, __builtin_ctzl)

CC_TRAILING_ZEROS_IMPL(cc_uint64, unsigned long long, __builtin_ctzll)

#undef CC_TRAILING_ZEROS_IMPL

#define cc_leading_zeros(X)                                                    \
  (_Generic((X), cc_uint8                                                      \
            : cc_leading_zeros_cc_uint8, cc_uint16                             \
            : cc_leading_zeros_cc_uint16, cc_uint32                            \
            : cc_leading_zeros_cc_uint32, cc_uint64                            \
            : cc_leading_cc_uint64)(X))

#define cc_trailing_zeros(X)                                                   \
  (_Generic((X), cc_uint8                                                      \
            : cc_trailing_zeros_cc_uint8, cc_uint16                            \
            : cc_trailing_zeros_cc_uint16, cc_uint32                           \
            : cc_trailing_zeros_cc_uint32, cc_uint64                           \
            : cc_trailing_zeros_cc_uint64)(X))

#endif // PROJECT_CL_BITS_H
