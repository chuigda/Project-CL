#ifndef PROJECT_CL_CC_ASSERT_H
#define PROJECT_CL_CC_ASSERT_H

#include <cc_defs.h>

#if !defined(PROJECT_CL_BUILD_ASSERT) || defined(NDEBUG)
#   define cc_assert(x)
#elif defined(PROJECT_CL_ASSERT_USE_LIBC)

#   include <assert.h>
#   include <cc_abort.h>
// since program maybe compiled with assume,
// we need to make the predicate opaque
#   define cc_assert(x)              \
    do {                             \
        if (!cc_opaque_predicate(x)) \
            cc_dump_stacktrace();    \
        assert(x);                   \
    } while (0)
#else
#   define cc_assert(x) cc_assert_impl((x), ##x, __FILE__, __LINE__);
extern "C" void cc_assert_impl(int value,
                               const char *expr,
                               const char *file,
                               int line);
#endif

#endif /* PROJECT_CL_CC_ASSERT_H */
