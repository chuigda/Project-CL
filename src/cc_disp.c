#include "cc_disp.h"
#include "cfg.h"

#if CCLIB_DISP == CC_FEAT_DISABLED
void cc_disp(const char* str) {
    (void)str;
}
void cc_disp_nonblock(const char *text) {
    (void)str;
}
void CCFN(cc_disp_ch) (char ch) {
    (void)ch;
}
void CCFN(cc_disp_ch_nonblock) (char ch) {
    (void)ch;
}
#elif CCLIB_DISP == CC_FEAT_USE_STD
#   include <stdio.h>
void CCFN(cc_disp) (const char *str) {
    fputs(str, stdout);
}
void CCFN(cc_disp_nonblock) (const char *str) {
    fputs(str, stderr);
}
void CCFN(cc_disp_ch) (char ch) {
    fputc(ch, stdout);
}
void CCFN(cc_disp_ch_nonblock) (char ch) {
    fputc(ch, stderr);
}
#elif CCLIB_DISP == CC_FEAT_USE_USER
#   include "cc_disp.user.c"
#endif
