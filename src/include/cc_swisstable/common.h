#ifndef PROJECT_CL_SWISSTABLE_COMMON_H
#define PROJECT_CL_SWISSTABLE_COMMON_H
#include "cc_swisstable/cc_swisstable.h"
#include <cc_defs.h>
typedef cc_uint64 cc_st_hash;
typedef cc_uint8 cc_st_ctrl;
#define CC_ST_EMPTY 0xFFU
#define CC_ST_DELETED 0x80U
#endif // PROJECT_CL_SWISSTABLE_COMMON_H
