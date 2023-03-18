#ifndef PROJECT_CL_CC_ABORT_H
#define PROJECT_CL_CC_ABORT_H

#include <cc_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

CC_ATTRIBUTE_COLD void cc_abort(void);

CC_ATTRIBUTE_COLD _Bool cc_dump_stacktrace(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_CC_ABORT_H */
