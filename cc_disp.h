#ifndef CC_DISP_H
#define CC_DISP_H

#include "cc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void CCFN(cc_disp) (const char *str);
extern void CCFN(cc_disp_nonblock) (const char *str);

extern void CCFN(cc_disp_ch) (char ch);
extern void CCFN(cc_disp_ch_nonblock) (char ch);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CC_DISP_H
