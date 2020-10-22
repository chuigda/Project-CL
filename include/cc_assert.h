#ifndef CCLIB_ASSERT_H
#define CCLIB_ASSERT_H

#include "cfg.h"
#include "cc_defs.h"
#ifdef __cplusplus
extern "C" {
#endif

extern void cclib_assert_impl(int value,
                               const char *expr,
                               const char *file,
                               unsigned line,
                               const char *text);

#ifndef CC_NDEBUG
#   define CC_ASSERT(expr) \
    cclib_assert_impl((expr), \
                      #expr, \
                      __FILE__, \
                      __LINE__, \
                      "assertion failed")
#   define CC_ASSERT2(expr, text) \
    cclib_assert_impl((expr), \
                      #expr, \
                      __FILE__, \
                      __LINE__, \
                      (text))
#else
#   define CC_ASSERT(expr)
#   define CC_ASSERT2(expr, text)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifndef NDEBUG
#   define CC_ASSUME(x) CC_ASSERT(x)
#else
#   if __has_builtin(__builtin_assume)
#       define CC_ASSUME(x) __builtin_assume((x))
#   elif defined(_MSC_VER)
#       define CC_ASSUME(x) __assume((x))
#   elif defined(__GNUC__)
#       define CC_ASSUME(x)       \
            if (!(x))             \
                __builtin_unreachable()
#   else
#       define CC_ASSUME(x) \
            do {            \
            } while (0)
#   endif
#endif
            
#ifdef CC_NDEBUG
#   define CC_UNREACHABLE CC_UNREACHABLE_HINT
#else
#   define CC_UNREACHABLE CC_PANIC ( );
#endif
            
#endif /* CCLIB_ASSERT_H */
