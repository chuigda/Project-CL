#include "test_kit.h"
#include <cc_defs.h>
#include <cc_encoding.h>
#include <cc_alloc.h>

#include <string.h>
#include <stdio.h>

const cc_char *test_strings[] = {
    "114514",
    "71RTS3SJXD", "SDS8YYDTM2", "Q0W2LU0IUV", "H1PM2M17YM", "O9VDGB66J9",
    "XY5MUSRXYC", "RKTVRZKEBJ", "RC4O73JTT0", "33YDLUVEPF", "2QMNTARBA1",
    "66HN0H1547", "P2JU82XHB9", "HEDFS2TVNG", "34DDI4SBG2", "WQPJWE4PPM"
};
const cc_char *test_b64[] = {
    "MTE0NTE0",
    "NzFSVFMzU0pYRA==", "U0RTOFlZRFRNMg==", "UTBXMkxVMElVVg==",
    "SDFQTTJNMTdZTQ==", "TzlWREdCNjZKOQ==", "WFk1TVVTUlhZQw==",
    "UktUVlJaS0VCSg==", "UkM0TzczSlRUMA==", "MzNZRExVVkVQRg==",
    "MlFNTlRBUkJBMQ==", "NjZITjBIMTU0Nw==", "UDJKVTgyWEhCOQ==",
    "SEVERlMyVFZORw==", "MzREREk0U0JHMg==", "V1FQSldFNFBQTQ=="
};

void test1(void) {
    cc_size olen;
    for (int i = 0; i < 15; i++) {
        cc_char *b64str = cc_base64_encode(test_strings[i], strlen(test_strings[i]), &olen);
        cc_assert(olen == strlen(test_b64[i]));
        cc_assert(strcmp(b64str, test_b64[i]) == 0);
        cc_assert(b64str[olen] == 0);
        cc_free(b64str);
    }
}

void test2(void) {
    cc_size olen;
    for (int i = 0; i < 15; i++) {
        cc_uchar *str = cc_base64_decode(test_b64[i], strlen(test_b64[i]), &olen);
        cc_assert(olen == strlen(test_strings[i]));
        cc_assert(memcmp(str, test_strings[i], olen) == 0);
        cc_free(str);
    }
}


BEGIN_TEST

    AUTO_TEST_ITEM(1)
    AUTO_TEST_ITEM(2)

END_TEST
