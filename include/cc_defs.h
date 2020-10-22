#ifndef CC_DEFS_H
#define CC_DEFS_H

#include "cfg.h"

#define CCFN(PROC_MACRO_FALLBACK) PROC_MACRO_FALLBACK
#define CCTY(PROC_MACRO_FALLBACK) PROC_MACRO_FALLBACK
#define CCRND8  4 /* PROC_MACRO_FALLBACK */
#define CCRND16 4 /* PROC_MACRO_FALLBACK */
#define CCRND32 4 /* PROC_MACRO_FALLBACK */
#define CCRND64 4 /* PROC_MACRO_FALLBACK */

#if !defined(CCLIB_SIZE_TYPE) && !defined(CCLIB_SSIZE_TYPE)
#   include <stddef.h>
typedef size_t CCTY(cc_size);
typedef ptrdiff_t CCTY(cc_ssize);
#else
typedef CCLIB_SIZE_TYPE CCTY(cc_size);
typedef CCLIB_SSIZE_TYPE CCTY(cc_ssize);
#endif

#define CC_OFFSETOF(a,b) ((CCTY(cc_ssize))(&(((a*)(0))->b)))
#define CC_VPTR_ADD(ptr, delta) \
    ((void *)((CCTY(cc_size))(ptr) + (CCTY(cc_size))(delta)))
#define CC_VPTR_SUB(ptr, delta) \
    ((void *)((CCTY(cc_size))(ptr) - (CCTY(cc_size))(delta)))
#define CC_VPTR_DIFF(x, y) ((CCTY(cc_ssize))(x) - (CCTY(cc_ssize))(y))

#ifndef NULL
#   define NULL ((void*)0)
#endif

#ifndef __has_builtin
#   define __has_builtin(x) 0
#endif

#if defined(__GNUC__) || defined(__clang__)
#   define CC_FAST_PATH        inline __attribute__((always_inline))
#   define CC_SLOW_PATH        __attribute__((noinline))
#   define CC_UNLIKELY(x)      __builtin_expect(!!(x),0)
#   define CC_LIKELY(x)        __builtin_expect(!!(x),1)
#   define CC_PURE             __attribute__((const))
#   ifndef __clang__
#       define CC_USE_GCC
#   endif
#   define CC_UNREACHABLE_HINT (__builtin_unreachable())
#elif defined(_MSC_VER)
#   define CC_FAST_PATH        inline __forceinline
#   define CC_SLOW_PATH        __declspec(noinline)
#   define CC_UNLIKELY         !!(x)
#   define CC_LIKELY           !!(x)
#   define CC_PURE             /* do nothing */
#   define CC_UNREACHABLE_HINT /* do nothing */
#else
#   define CC_FAST_PATH        inline
#   define CC_SLOW_PATH        /* do nothing */
#   define CC_UNLIKELY(x)      !!(x)
#   define CC_LIKELY(x)        !!(x)
#   define CC_PURE             /* do nothing */
#   define CC_UNREACHABLE_HINT
#endif

#define CC_UNUSED(x) ((void)(x))

#endif /* CC_DEFS_H */
