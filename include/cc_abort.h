#ifndef PROJECT_CL_CC_ABORT_H
#define PROJECT_CL_CC_ABORT_H

#include <cc_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

#if PROJECT_CL_HAS_ATTRIBUTE(cold)
__attribute__((cold))
#endif
void cc_abort();

#if PROJECT_CL_HAS_ATTRIBUTE(cold)
__attribute__((cold))
#endif
_Bool cc_dump_stacktrace();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_CC_ABORT_H */
