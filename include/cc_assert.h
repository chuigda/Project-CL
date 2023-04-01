#ifndef PROJECT_CL_CC_ASSERT_H
#define PROJECT_CL_CC_ASSERT_H

#include <cc_defs.h>

#if !defined(PROJECT_CL_BUILD_ASSERT) || defined(NDEBUG)
#   define cc_assert(x)
#elif defined(PROJECT_CL_ASSERT_USE_LIBC)
#   include <assert.h>
#   define cc_assert assert
#else
#   define cc_assert(x) cc_assert_impl((int)(cc_size)(x), #x, __FILE__, __LINE__);
#ifdef __cplusplus
extern "C"
#endif
void cc_assert_impl(int value,
                    const char *expr,
                    const char *file,
                    int line);
#endif

#endif /* PROJECT_CL_CC_ASSERT_H */
