#include "cc_assert.h"
#include "cc_inttypes.h"
#include "cc_disp.h"
#include "cc_panic.h"

#define CC_DISP_NONBLOCK CCFN(cc_disp_nonblock)
#define CC_DISP_CH_NONBLOCK CCFN(cc_disp_ch_nonblock)
#define CC_PANIC CCFN(cc_panic)

static void mini_display_u32(CCTY(cc_uint32) u) {
    char buf[11];
    buf[10] = '\0';
    CCTY(cc_uint8) idx = 9;
    do {
        buf[idx] = u % 10 + '0';
        u = u / 10;
        idx = idx - 1;
    } while (u != 0);
    CC_DISP_NONBLOCK (buf + idx);
}

void cclib_assert_impl(int value,
                       const char *expr,
                       const char *file,
                       unsigned line,
                       const char *text) {
    if (value == 0) {
        CC_DISP_NONBLOCK ("assert failed at file \"");
        CC_DISP_NONBLOCK (file);
        CC_DISP_NONBLOCK ("\", line ");
        mini_display_u32((CCTY(cc_uint32))line);
        CC_DISP_NONBLOCK (": ");
        CC_DISP_NONBLOCK (expr);
        CC_DISP_NONBLOCK (": ");
        CC_DISP_NONBLOCK (text);
        CC_DISP_CH_NONBLOCK ('\n');
        CC_PANIC ();
    }
}
