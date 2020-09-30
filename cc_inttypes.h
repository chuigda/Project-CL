#ifndef CC_INTTYPES_H
#define CC_INTTYPES_H

#include "cfg.h"
#include "cc_defs.h"

#if CCLIB_INTLIB == CC_FEAT_USE_USER
#   include "cc_inttypes.user.h"
#elif CCLIB_INTLIB == CC_FEAT_USE_STD
#   include <stdint.h>
typedef int8_t CCTY(cc_int8);
typedef int16_t CCTY(cc_int16);
typedef int32_t CCTY(cc_int32);
typedef int64_t CCTY(cc_int64);
typedef uint8_t CCTY(cc_uint8);
typedef uint16_t CCTY(cc_uint16);
typedef uint32_t CCTY(cc_uint32);
typedef uint64_t CCTY(cc_uint64);
#endif

#endif // CC_INTTYPES_H
