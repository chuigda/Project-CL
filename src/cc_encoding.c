#include <cc_cfg.h>

#ifdef PROJECT_CL_BUILD_ENCODING

#include <cc_encoding.h>
#include <cc_alloc.h>


static const cc_char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const cc_int32 base64_index[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
    7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
    0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

cc_char *cc_base64_encode(const void *src, const cc_size len, cc_size *o_len) {
    cc_size olen = 4 * ((len + 2) / 3); /* 3-byte blocks to 4-byte */

    if (olen < len)
        return NULL; /* integer overflow */

    cc_char *out = cc_alloc(sizeof(cc_uchar) * (olen + 1));
    if (!out)
        return NULL;

    const cc_uchar *in = src;
    const cc_uchar *end = in + len;
    cc_char *pos = out;
    while (end - in >= 3) {
        *pos++ = base64_table[in[0] >> 2];
        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = base64_table[in[2] & 0x3f];
        in += 3;
    }

    if (end - in) {
        *pos++ = base64_table[in[0] >> 2];
        if (end - in == 1) {
            *pos++ = base64_table[(in[0] & 0x03) << 4];
            *pos++ = '=';
        }
        else {
            *pos++ = base64_table[((in[0] & 0x03) << 4) |
                (in[1] >> 4)];
            *pos++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
    }

    *pos = 0;
    if (o_len) *o_len = olen;
    return out;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wchar-subscripts"
cc_uchar *cc_base64_decode(const cc_char *data, const cc_size len, cc_size *o_len) {
    const cc_char *p = data;
    cc_int32 pad = len > 0 && (len % 4 || p[len - 1] == '=');
    const cc_size L = ((len + 3) / 4 - pad) * 4;
    cc_size olen = L / 4 * 3 + pad;
    cc_uchar *res = cc_alloc(sizeof(cc_uchar) * (olen + 2));
    for (cc_size i = 0; i < olen + 2; i++) {
        res[i] = 0;
    }

    for (size_t i = 0, j = 0; i < L; i += 4) {
        cc_int32 n = (
            base64_index[p[i]] << 18
            | base64_index[p[i + 1]] << 12
            | base64_index[p[i + 2]] << 6
            | base64_index[p[i + 3]]
        );
        res[j++] = n >> 16;
        res[j++] = n >> 8 & 0xFF;
        res[j++] = n & 0xFF;
    }
    if (pad) {
        int n = base64_index[p[L]] << 18 | base64_index[p[L + 1]] << 12;
        res[olen - 1] = n >> 16;

        if (len > L + 2 && p[L + 2] != '=')
        {
            n |= base64_index[p[L + 2]] << 6;
            res[olen] = n >> 8 & 0xFF;
            olen++;
        }
    }
    *o_len = olen;
    return res;
}
#pragma GCC diagnostic pop

#endif
