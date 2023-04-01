#include <cc_cfg.h>

#if !defined(PROJECT_CL_BUILD_ALLOC)
/* simply do nothing, not using alloc */
#elif defined(PROJECT_CL_ALLOC_USE_EXTERNAL)
/* simply do nothing, alloc function definitions will be linked in */
#else 
#   include <cc_alloc.h>
#   if defined(PROJECT_CL_ALLOC_INCLUDE_IMPL)
#       include "alloc_impl.h"
#   elif defined(PROJECT_CL_ALLOC_USE_LIBC)
#       include <stdlib.h>
void *cc_alloc(cc_size size) {
    return malloc(size);
}

void *cc_alloc2(cc_size size, cc_size align) {
#       if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    return aligned_alloc(align, size);
#       else
    (void)align;
    return malloc(size);
#       endif
}

void *cc_realloc(void *ptr, cc_size size) {
    return realloc(ptr, size);
}

void cc_free(void *ptr) {
    free(ptr);
}
#   elif defined(PROJECT_CL_ALLOC_USE_VALLOC)
cc_uint8 g_valloc_buf[PROJECT_CL_VALLOC_BUFSIZ];

cc_size cc_valloc_bufsiz(void) {
    return PROJECT_CL_VALLOC_BUFSIZ;
}

/* TODO implement valloc */
#   else
#       error PROJECT_CL_BUILD_ALLOC enabled without valid sub-option
#   endif
#endif
