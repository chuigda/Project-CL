#ifndef PROJECT_CL_ALLOC_H
#define PROJECT_CL_ALLOC_H

#include <cc_defs.h>

#ifndef PROJECT_CL_BUILD_ALLOC
#   error PROJECT_CL_BUILD_ALLOC not defined
#endif

#ifdef __cplusplus
extern "C" {
#endif

void *cc_alloc(cc_size size);

void *cc_alloc2(cc_size size, cc_size align);

void *cc_realloc(void *ptr, cc_size size);

void cc_free(void *ptr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_ALLOC_H */
