#ifndef PROJECT_CL_CC_ASSERT_H
#define PROJECT_CL_CC_ASSERT_H

#include <cc_defs.h>

#if !defined(PROJECT_CL_BUILD_ASSERT) || defined(NDEBUG)
#   define cc_assert(x)
#elif defined(PROJECT_CL_ASSERT_USE_LIBC)
#   include <assert.h>
#   define cc_assert assert
#else
#   define cc_assert(x)                                  \
  (CC_LIKELY(x)                                          \
       ? (void)(0)                                       \
       : cc_assert_failure_impl(#x, __FILE__, __LINE__));
#ifdef __cplusplus
extern "C"
#endif
CC_ATTRIBUTE_EXPORT CC_ATTRIBUTE_COLD
void cc_assert_failure_impl(const char *expr,
                            const char *file,
                           int line);
#endif

#endif /* PROJECT_CL_CC_ASSERT_H */
