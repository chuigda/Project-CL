#ifndef PROJECT_CL_SWISSTABLE_COMMON_H
#define PROJECT_CL_SWISSTABLE_COMMON_H
#include <cc_defs.h>
typedef cc_uint64 cc_st_hash;
typedef cc_uint8 cc_st_ctrl;
#define CC_ST_EMPTY 0b11111111u
#define CC_ST_DELETED 0b10000000u
#endif // PROJECT_CL_SWISSTABLE_COMMON_H
