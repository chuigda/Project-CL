#include <cc_abort.h>

#if defined(PROJECT_CL_ABORT_USE_EXTERNAL)
/* simply do nothing, abort function definition will be linked in */
#elif defined(PROJECT_CL_ABORT_USE_INCLUDE_IMPL)
#   include "abort_impl.h"
#elif defined(PROJECT_CL_ABORT_USE_LIBC)

#   include <stdlib.h>

CC_ATTRIBUTE_COLD void cc_abort(void) {
    abort();
}

#elif defined(PROJECT_CL_ABORT_USE_DEAD_LOOP)

CC_ATTRIBUTE_COLD void cc_abort(void) {
    /* https://stackoverflow.com/questions/66109167 */
    volatile int spin = 1;
    while (*(volatile int*)&spin) {}
}

#else
#   error no valid PROJECT_CL_ABORT_* suboption
#endif

#if defined(__has_include) && __has_include(<execinfo.h>) && __has_include(<unistd.h>)

#include <execinfo.h>
#include <unistd.h>

_Bool cc_dump_stacktrace() {
    void *array[64];
    int size;
    size = backtrace(array, 64);
    write(0, "\n\n", 2);
    backtrace_symbols_fd(array, size, 0);
    write(0, "\n", 1);
    return 1;
}

#else
_Bool cc_dump_stacktrace() {
    return 0;
}
#endif
