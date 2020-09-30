#include "cc_panic.h"
#include "cfg.h"

#if CCLIB_PANIC == CC_FEAT_USE_STD
#   include <stdlib.h>
void CCFN(cc_panic) (void) {
  abort();
}
#elif CCLIB_PANIC == CC_FEAT_USE_USER
#   include "cc_panic.user.c"
#endif
