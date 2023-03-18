#include <cc_hash.h>
#include "cc_hash/common.h"
#include "cc_hash/asimd.h"
#define CC_INTERNAL_IS_BIG_ENDIAN() (!*(unsigned char *)&(uint16_t){1})
/* emulate uint128 for internal use */
typedef struct {
    union {
        _Alignas(16) cc_uint64 data[2];
#ifdef __SIZEOF_INT128__
        unsigned __int128 whole_data;
#endif
    };
} cc_impl_uint128;

#ifndef __SIZEOF_INT128__
static inline cc_uint64 *cc_internal_uin128_low(cc_impl_uint128 *x) {
    return &x->data[CC_INTERNAL_IS_BIG_ENDIAN()];
}

static inline cc_uint64 *cc_internal_uin128_high(cc_impl_uint128 *x) {
    return &x->data[!CC_INTERNAL_IS_BIG_ENDIAN()];
}

static inline void cc_internal_full_mul(cc_uint64 op1, cc_uint64 op2, cc_uint64 *hi, cc_uint64 *lo) {
    cc_uint64 u1 = (op1 & 0xffffffff);
    cc_uint64 v1 = (op2 & 0xffffffff);
    cc_uint64 t = (u1 * v1);
    cc_uint64 w3 = (t & 0xffffffff);
    cc_uint64 k = (t >> 32);

    op1 >>= 32;
    t = (op1 * v1) + k;
    k = (t & 0xffffffff);
    uint64_t w1 = (t >> 32);

    op2 >>= 32;
    t = (u1 * op2) + k;
    k = (t >> 32);

    *hi = (op1 * op2) + w1 + k;
    *lo = (t << 32) + w3;
}
#endif

cc_impl_uint128 cc_impl_uint128_multiply(cc_impl_uint128 a, cc_impl_uint128 b) {
    cc_impl_uint128 result;
#ifdef __SIZEOF_INT128__
    result.whole_data = a.whole_data * b.whole_data;
#else
    cc_uint64 hi, lo;
    cc_internal_full_mul(
            *cc_internal_uin128_low(&a), *cc_internal_uin128_low(&b), &hi, &lo);
    hi += (*cc_internal_uin128_high(&a)) * (*cc_internal_uin128_low(&b));
    hi += (*cc_internal_uin128_high(&b)) * (*cc_internal_uin128_low(&a));
    *cc_internal_uin128_high(&result) = hi;
    *cc_internal_uin128_low(&result) = lo;
#endif
    return result;
}
