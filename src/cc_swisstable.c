#include <cc_cfg.h>
#include "cc_bits.h"
#include "cc_swisstable/common.h"

#define CC_ST_WORD_MASK  0xFFFF
#define CC_ST_WORD_STRIDE 1
typedef cc_uint16 cc_st_bitmask_impl;

#include "cc_swisstable/bitmask.inc"

void place_holder(void) {

}
