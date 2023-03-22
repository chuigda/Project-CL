#ifndef PROJECT_CL_IMPL_H
#define PROJECT_CL_IMPL_H

#ifdef PROJECT_CL_RUNTIME_CHECK
#   define RT_CHECK(X) X
#else
#   define RT_CHECK(X)
#endif

#define RT_CONTRACT(C) \
    RT_CHECK({ \
        _Bool v = C; \
        cc_assert(v && "CONTRACT " #C); \
        if (!v) { \
            return; \
        } \
    })

#define RT_CONTRACT2(C, R) \
    RT_CHECK({ \
        _Bool v = C; \
        cc_assert(v && "CONTRACT " #C); \
        if (!v) { \
            return R; \
        } \
    })

#define RT_CONTRACT_E(C) \
    RT_CHECK({ \
        _Bool v = C; \
        cc_assert(v && "CONTRACT " #C); \
        if (!v) { \
            return CC_ASSERTION_WOULD_FAIL; \
        } \
    })

#endif /* PROJECT_CL_IMPL_H */

