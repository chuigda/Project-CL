#ifndef CCLIB_ASSERT_H
#define CCLIB_ASSERT_H

#include "cfg.h"

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

#endif /* CCLIB_ASSERT_H */
