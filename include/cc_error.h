#ifndef PROJECT_CL_ERROR_H
#define PROJECT_CL_ERROR_H

#include <cc_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    CC_NO_ERROR = 0,
    CC_INVALID_ENUM = 0x0500,
    CC_INVALID_VALUE = 0x0501,
    CC_INVALID_OPERATION = 0x0502,
    CC_OUT_OF_MEMORY = 0x0505,
    CC_END_OF_ITERATOR = 0x0540,
    CC_BAD_UTF8_BYTE = 0x0550,
    CC_IO_ERROR = 0x0598,
    CC_UNIMPLEMENTED = 0x1145,
    CC_ASSERTION_WOULD_FAIL = 0x1146,
    CC_UNKNOWN_ERROR = -1
};

typedef cc_int32 cc_error;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_ERROR_H */
