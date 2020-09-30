#ifndef CC_DEFS_H
#define CC_DEFS_H

#include "cfg.h"

#define CCFN(X_SOME_FN) X_SOME_FN
#define CCTY(X_SOME_TYPE) X_SOME_TYPE

#if !defined(CCLIB_SIZE_TYPE) && !defined(CCLIB_SSIZE_TYPE)
#   include <stddef.h>
typedef size_t CCTY(cc_size);
typedef ptrdiff_t CCTY(cc_ssize);
#else
typedef CCLIB_SIZE_TYPE CCTY(cc_size);
typedef CCLIB_SSIZE_TYPE CCTY(cc_ssize);
#endif

#ifndef NULL
#   define NULL ((void*)0)
#endif

#endif // CC_DEFS_H
