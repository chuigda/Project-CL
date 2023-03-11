#include <defs.h>

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
#define ISUNSIGNED(T) ((T) -1 > 0)
#   define ISSIGNED(T) (!ISUNSIGNED(T))
_Static_assert(sizeof(cc_int8) == 1,
               "SANITY CHECK: size of cc_int8 should be 1 byte");
_Static_assert(ISSIGNED(cc_int8),
               "SANITY_CHECK: cc_int8 should be signed");
_Static_assert(sizeof(cc_uint8) == 1,
               "SANITY CHECK: size of cc_uint8 should be 1 byte");
_Static_assert(ISUNSIGNED(cc_uint8),
               "SANITY CHECK: cc_uint8 should be unsigned");
#endif
