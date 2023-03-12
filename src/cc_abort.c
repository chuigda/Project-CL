#include <cc_abort.h>

#if defined(PROJECT_CL_ABORT_USE_EXTERNAL)
/* simply do nothing, abort function definition will be linked in */
#elif defined(PROJECT_CL_ABORT_USE_INCLUDE_IMPL)
#   include "abort_impl.h"
#elif defined(PROJECT_CL_ABORT_USE_LIBC)
#   include <stdlib.h>
void cc_abort(void) {
    abort();
}
#elif defined(PROJECT_CL_ABORT_USE_DEAD_LOOP)
void cc_abort(void) {
    /* https://stackoverflow.com/questions/66109167 */
    volatile int spin = 1;
    while (*(volatile int*)&spin) {}
}
#else
#   error no valid PROJECT_CL_ABORT_* suboption
#endif
