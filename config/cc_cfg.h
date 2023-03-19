#ifndef PROJECT_CL_CC_CFG_H
#define PROJECT_CL_CC_CFG_H

/* note: this is just a piece of example configuration, if you want to 
   customise Project-CL, you need to write or generate your own one. */

#define PROJECT_CL_ABORT_USE_LIBC
#define PROJECT_CL_BUILD_ASSERT
#define PROJECT_CL_ASSERT_USE_LIBC
#define PROJECT_CL_MEMORY_USE_LIBC
#define PROJECT_CL_BUILD_ALLOC
#define PROJECT_CL_ALLOC_USE_LIBC
#define PROJECT_CL_BUILD_VEC
#define PROJECT_CL_BUILD_LIST
#define PROJECT_CL_BUILD_SET
#define PROJECT_CL_BILD_MAP
#define PROJECT_CL_BUILD_STRING
#define PROJECT_CL_BUILD_MSTRING
#define PROJECT_CL_BUILD_PROBE
#define PROJECT_CL_BUILD_PL5
#define PROJECT_CL_USE_INT64

#define PROJECT_CL_RUNTIME_CHECK

#define PROJECT_CL_UNUSED(X) ((void)(X))
#define PROJECT_CL_IS_BIG_ENDIAN() (!*(unsigned char *)&(uint16_t){1})

#endif /* PROJECT_CL_CC_CFG_H */
