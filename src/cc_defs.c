#include <cc_defs.h>

/* This file does nothing, but configuration sanity check */

#if !defined(PROJECT_CL_ABORT_USE_EXTERNAL) \
    && !defined(PROJECT_CL_ABORT_INCLUDE_IMPL) \
    && !defined(PROJECT_CL_ABORT_USE_LIBC) \
    && !defined(PROJECT_CL_ABORT_USE_DEAD_LOOP)
#   error SANITY CHECK: no valid PROJECT_CL_ABORT_* suboption
#endif

#ifndef PROJECT_CL_BUILD_ALLOC
#   ifdef PROJECT_CL_BUILD_VEC
#       error SANITY CHECK: cannot enable VEC without ALLOC
#   endif
#   ifdef PROJECT_CL_BUILD_LIST
#       error SANITY CHECK: cannot enable LIST without ALLOC
#   endif
#   ifdef PROJECT_CL_BUILD_SET
#       error SANITY CHECK: cannot enable SET without ALLOC
#   endif
#   ifdef PROJECT_CL_BUILD_MAP
#       error SANITY CHECK: cannot enable MAP without ALLOC
#   endif
#   ifdef PROJECT_CL_BUILD_STRING
#       error SANITY CHECK: cannot enable STRING without ALLOC
#   endif
#   ifdef PROJECT_CL_BUILD_MSTRING
#       error SANITY CHECK: cannot enable MSTRING without ALLOC
#   endif
#endif

#ifdef PROJECT_CL_BUILD_ALLOC
#   if !defined(PROJECT_CL_ALLOC_USE_EXTERNAL) \
    && !defined(PROJECT_CL_ALLOC_INCLUDE_IMPL) \
    && !defined(PROJECT_CL_ALLOC_USE_LIBC) \
    && !defined(PROJECT_CL_ALLOC_USE_VALLOC)
#       error SANITY CHECK: ALLOC enabled without any valid sub-option
#   endif
#endif

#ifdef PROJECT_CL_ALLOC_USE_VALLOC
#   ifndef PROJECT_CL_VALLOC_BUFSIZ
#       error SANITY CHECK: VALLOC enabled without a valid \
PROJECT_CL_VALLOC_BUFSIZ value defined
#   endif
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
/* https://stackoverflow.com/questions/7469915 */
#   define ISUNSIGNED(T) ((T) -1 > 0)
#   define ISSIGNED(T) (!ISUNSIGNED(T))
_Static_assert(ISSIGNED(cc_isize), "isize must be signed");
_Static_assert(ISUNSIGNED(cc_size), "size must be unsigned");
_Static_assert(sizeof(cc_int8) == 1 && ISSIGNED(cc_int8), "cc_int8 should be of size 1 and should be signed");
_Static_assert(sizeof(cc_uint8) == 1 && ISUNSIGNED(cc_uint8), "cc_uint8 should be of size 1 and should be unsigned");
_Static_assert(sizeof(cc_int16) == 2 && ISSIGNED(cc_int16), "cc_int16 should be of size 2 and should be signed");
_Static_assert(sizeof(cc_uint16) == 2 && ISUNSIGNED(cc_uint16), "cc_uint16 should be of size 2 and should be unsigned");
_Static_assert(sizeof(cc_int32) == 4 && ISSIGNED(cc_int32), "cc_int32 should be of size 4 and should be signed");
_Static_assert(sizeof(cc_uint32) == 4 && ISUNSIGNED(cc_uint32), "cc_uint32 should be of size 4 and should be unsigned");
#   ifdef PROJECT_CL_USE_INT64
_Static_assert(sizeof(cc_int64) == 8 && ISSIGNED(cc_int64), "cc_int64 should be of size 8 and should be signed");
_Static_assert(sizeof(cc_uint64) == 8 && ISUNSIGNED(cc_uint64), "cc_uint64 should be of size 8 and should be unsigned");
#   endif
#   ifdef PROJECT_CL_USE_INT128
_Static_assert(sizeof(cc_int128) == 16 && ISSIGNED(cc_int128), "cc_int128 should be of size 16 and should be signed");
_Static_assert(sizeof(cc_uint128) == 16 && ISUNSIGNED(cc_uint128), "cc_uint128 should be of size 16 and should be unsigned");
#   endif

#   ifdef PROJECT_CL_FP16_TYPE
_Static_assert(sizeof(cc_f16) == 2);
#   endif
_Static_assert(sizeof(cc_f32) == 4, "cc_f32 should be of size 4");
_Static_assert(sizeof(cc_f64) == 8, "cc_f32 should be of size 8");
#   ifdef PROJECT_CL_FP128_TYPE
_Static_assert(sizeof(cc_f128) == 16, "cc_f32 should be of size 16");
#   endif
#endif
