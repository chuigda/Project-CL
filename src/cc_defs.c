#include <cc_defs.h>

/* This file does nothing, but configuration sanity check */

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
_Static_assert(ISSIGNED(cc_isize));
_Static_assert(ISUNSIGNED(cc_size));
_Static_assert(sizeof(cc_int8) == 1 && ISSIGNED(cc_int8));
_Static_assert(sizeof(cc_uint8) == 1 && ISUNSIGNED(cc_uint8));
_Static_assert(sizeof(cc_int16) == 2 && ISSIGNED(cc_int16));
_Static_assert(sizeof(cc_uint16) == 2 && ISUNSIGNED(cc_uint16));
_Static_assert(sizeof(cc_int32) == 4 && ISSIGNED(cc_int32));
_Static_assert(sizeof(cc_uint32) == 4 && ISUNSIGNED(cc_uint32));
#   ifdef PROJECT_CL_USE_INT64
_Static_assert(sizeof(cc_int64) == 8 && ISSIGNED(cc_int64));
_Static_assert(sizeof(cc_uint64) == 8 && ISUNSIGNED(cc_uint64));
#   endif
#   ifdef PROJECT_CL_USE_INT128
_Static_assert(sizeof(cc_int128) == 16 && ISSIGNED(cc_int128));
_Static_assert(sizeof(cc_uint128) == 16 && ISUNSIGNED(cc_uint128));
#   endif

#   ifdef PROJECT_CL_FP16_TYPE
_Static_assert(sizeof(cc_f16) == 2);
#   endif
_Static_assert(sizeof(cc_f32) == 4);
_Static_assert(sizeof(cc_f64) == 8);
#   ifdef PROJECT_CL_FP128_TYPE
_Static_assert(sizeof(cc_f128) == 16);
#   endif
#endif
