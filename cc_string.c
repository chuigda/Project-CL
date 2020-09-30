#include "cc_string.h"
#include "cfg.h"

#if CCLIB_STRLIB == CC_FEAT_USE_STD
#   include "cc_string.std.c"
#elif CCLIB_STRLIB == CC_FEAT_USE_CCLIB
#   include "cc_string.cclib.c"
#endif
