#include "cc_alloc.h"

#if CCLIB_ALLOC == CC_FEAT_DISABLED
#elif CCLIB_ALLOC == CC_FEAT_USE_STD
#   include "cc_alloc.std.c"
#elif CCLIB_ALLOC == CC_FEAT_USE_CCLIB
#   include "cc_alloc.cclib.c"
#elif CCLIB_ALLOC == CC_FEAT_USE_USER
#   include "cc_alloc.user.c"
#endif
