#ifndef PROJECT_CL_DEFS_H
#define PROJECT_CL_DEFS_H

#if (defined(__has_include) && __has_include(<cc_cfg.h>)) \
    || PROJECT_CL_CFG
#   include <cc_cfg.h>
#endif

#ifndef PROJECT_CL_DONT_USE_STDDEF
#   include <stddef.h>
#endif

#ifndef PROJECT_CL_DONT_USE_STDINT
#   include <stdint.h>
#endif

#ifdef PROJECT_CL_SIZE_TYPE
typedef PROJECT_CL_SIZE_TYPE cc_size;
#else
typedef size_t cc_size;
#endif

#ifdef PROJECT_CL_ISIZE_TYPE
typedef PROJET_CL_ISIZE_TYPE cc_isize;
#else
typedef ptrdiff_t cc_isize;
#endif

#ifdef PROJECT_CL_INT8_TYPE
typedef PROJECT_CL_INT8_TYPE cc_int8;
#else
typedef int8_t cc_int8;
#endif

#ifdef PROJECT_CL_UINT8_TYPE
typedef PROJECT_CL_UINT8_TYPE cc_uint8;
#else
typedef uint8_t cc_uint8;
#endif

#endif /* PROJECT_CL_DEFS_H */
