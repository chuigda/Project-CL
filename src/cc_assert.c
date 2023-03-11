#include <cc_assert.h>

#if defined(PROJECT_CL_BUILD_ASSERT) && \
    !defined(NDEBUG) && \
    !defined(PROJECT_CL_ASSERT_USE_LIBC)
/* TODO implement cc_assert_impl, would require other things */
#endif