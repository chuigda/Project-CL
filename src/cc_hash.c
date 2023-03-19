#include "cc_hash/asimd.h"
#include "cc_hash/common.h"
#include "cc_hash/ssse3.h"
#include "cc_hash/sve.h"
#include "cc_hash/vaes.h"
#include "cc_impl.h"
#include <cc_hash.h>

static cc_uint64 PI[] = {
        0x243f6a8885a308d3,
        0x13198a2e03707344,
        0xa4093822299f31d0,
        0x082efa98ec4e6c89,
};

static cc_uint64 PI2[] = {
        0x452821e638d01377,
        0xbe5466cf34e90c6c,
        0xc0ac29b7c97c50dd,
        0x3f84d5b5b5470917,
};

typedef struct {
    cc_uint64 data[2];
} cc_hash_small_data;

static inline
cc_hash_small_data cc_hash_load_small_data(const void *source, cc_size length) {
    cc_hash_small_data buffer;
    const uint8_t *data = (const uint8_t *) source;
    if (length >= 2) {
        if (length >= 4) {
            PROJECT_CL_HASH_GENERIC_LOAD(&buffer.data[0], &data[0]);
            PROJECT_CL_HASH_GENERIC_LOAD(&buffer.data[1], &data[length - 4]);
        } else {
            cc_uint16 head;
            cc_uint8 tail;
            PROJECT_CL_HASH_GENERIC_LOAD(&head, &data[0]);
            tail = data[length - 1];
            buffer.data[0] = (cc_uint64) head;
            buffer.data[1] = (cc_uint64) tail;
        }
    } else {
        if (length > 0) {
            buffer.data[0] = (cc_uint64) data[0];
            buffer.data[1] = (cc_uint64) data[0];
        } else {
            buffer.data[0] = 0;
            buffer.data[1] = 1;
        }
    }
    return buffer;
}

#if PROJECT_CL_HASH_HAS_BASIC_SIMD
typedef struct {
    cc_hashvec key;
    cc_hashvec enc;
    cc_hashvec sum;
} cc_vhasher;

static inline void cc_hash_digest_hashvec(cc_vhasher *hasher, cc_hashvec state) {
    hasher->enc = cc_hash_simd_encode(hasher->enc, state);
    hasher->sum = cc_hash_simd_shuffle_add(hasher->sum, state);
}

static inline void cc_hash_digest_hashvec2(cc_vhasher *hasher, cc_hashvec x, cc_hashvec y) {
    cc_hash_digest_hashvec(hasher, x);
    cc_hash_digest_hashvec(hasher, y);
}

#define PROJECT_CL_GENERATE_VECTORIZED_DIGEST(UTYPE, DTYPE, PREFIX)            \
  static inline _Bool PREFIX##_vectorized_digest(                              \
      cc_vhasher *hasher, const cc_uint8 *data, cc_size length) {              \
    const UTYPE duplicated_key = PREFIX##_simd_broadcast(hasher->key);         \
    const cc_size num_lanes = PREFIX##_simd_lanes(duplicated_key);             \
    if (4 * num_lanes >= length)                                               \
      return 0;                                                                \
    const UTYPE tail0 = PREFIX##_simd_uload(&data[length - 4 * num_lanes]);    \
    const UTYPE tail1 = PREFIX##_simd_uload(&data[length - 3 * num_lanes]);    \
    const UTYPE tail2 = PREFIX##_simd_uload(&data[length - 2 * num_lanes]);    \
    const UTYPE tail3 = PREFIX##_simd_uload(&data[length - 1 * num_lanes]);    \
    UTYPE current0 = PREFIX##_simd_encode(duplicated_key, tail0);              \
    UTYPE current1 = PREFIX##_simd_encode(duplicated_key, tail1);              \
    UTYPE current2 = PREFIX##_simd_encode(duplicated_key, tail2);              \
    UTYPE current3 = PREFIX##_simd_encode(duplicated_key, tail3);              \
    UTYPE sum0 = PREFIX##_simd_shuffle_add(                                    \
        PREFIX##_simd_add64(duplicated_key, tail0), tail2);                    \
    UTYPE sum1 = PREFIX##_simd_shuffle_add(                                    \
        PREFIX##_simd_add64(duplicated_key, tail1), tail3);                    \
    while (length > 4 * num_lanes) {                                           \
      const UTYPE head0 = PREFIX##_simd_uload(&data[0 * num_lanes]);           \
      const UTYPE head1 = PREFIX##_simd_uload(&data[1 * num_lanes]);           \
      const UTYPE head2 = PREFIX##_simd_uload(&data[2 * num_lanes]);           \
      const UTYPE head3 = PREFIX##_simd_uload(&data[3 * num_lanes]);           \
      current0 = PREFIX##_simd_encode(current0, head0);                        \
      current1 = PREFIX##_simd_encode(current1, head1);                        \
      current2 = PREFIX##_simd_encode(current2, head2);                        \
      current3 = PREFIX##_simd_encode(current3, head3);                        \
      sum0 = PREFIX##_simd_shuffle_add(sum0, head0);                           \
      sum1 = PREFIX##_simd_shuffle_add(sum1, head1);                           \
      sum0 = PREFIX##_simd_shuffle_add(sum0, head2);                           \
      sum1 = PREFIX##_simd_shuffle_add(sum1, head3);                           \
      data += 4 * num_lanes;                                                   \
      length -= 4 * num_lanes;                                                 \
    }                                                                          \
    const UTYPE encoded0 = PREFIX##_simd_encode(current0, current1);           \
    const UTYPE encoded1 = PREFIX##_simd_encode(current2, current3);           \
    const UTYPE total = PREFIX##_simd_add64(sum0, sum1);                       \
    for (size_t i = 0; i < num_lanes / 16; ++i) {                              \
      DTYPE a = PREFIX##_simd_downcast(encoded0, i);                           \
      DTYPE b = PREFIX##_simd_downcast(encoded1, i);                           \
      DTYPE c = PREFIX##_simd_downcast(total, i);                              \
      cc_hash_digest_hashvec2(hasher, a, b);                                   \
      cc_hash_digest_hashvec(hasher, c);                                       \
    }                                                                          \
    return 1;                                                                  \
  }

PROJECT_CL_GENERATE_VECTORIZED_DIGEST(cc_hashvec, cc_hashvec, cc_hash)

#if PROJECT_CL_HASH_HAS_WIDE_SIMD

PROJECT_CL_GENERATE_VECTORIZED_DIGEST(cc_wide_hashvec, cc_hashvec, cc_hash_wide)

#endif

cc_uint64 cc_finalize_hasher(const cc_hasher *hasher) {
    const cc_vhasher *vhasher = (const cc_vhasher *) hasher;
    cc_hashvec combined = cc_hash_simd_decode(vhasher->sum, vhasher->enc);
    cc_hashvec result = cc_hash_simd_encode(cc_hash_simd_encode(combined, vhasher->key), combined);
    return cc_hash_simd_lower_half(result);
}

static inline
cc_uint64 cc_hash_mix_integer(cc_vhasher hasher, cc_uint64 x, cc_uint64 y) {
    cc_hash_digest_hashvec(&hasher, cc_hash_simd_u64x2(x, 0));
    cc_hash_digest_hashvec(&hasher, cc_hash_simd_u64x2(y, 0));
    return cc_finalize_hasher((const cc_hasher *) &hasher);
}

void cc_initialize_hasher(cc_hasher *hasher, cc_uint64 seed) {
    cc_vhasher *vhasher = (cc_vhasher *) hasher;
    vhasher->enc = cc_hash_simd_u64x2(PI[0], PI[1]);
    vhasher->sum = cc_hash_simd_u64x2(PI[2], PI[3]);
    vhasher->key = cc_hash_simd_xor(vhasher->enc, vhasher->sum);
    cc_hash_digest_hashvec(vhasher, cc_hash_simd_u64x2(seed, 0));
    cc_uint64 mixed[4] = {
            cc_hash_mix_integer(*vhasher, PI2[0], PI2[2]),
            cc_hash_mix_integer(*vhasher, PI2[1], PI2[3]),
            cc_hash_mix_integer(*vhasher, PI2[2], PI2[1]),
            cc_hash_mix_integer(*vhasher, PI2[3], PI2[0]),
    };
    vhasher->enc = cc_hash_simd_u64x2(mixed[0], mixed[1]);
    vhasher->sum = cc_hash_simd_u64x2(mixed[2], mixed[3]);
    vhasher->key = cc_hash_simd_xor(vhasher->enc, vhasher->sum);
}

void cc_hasher_digest(cc_hasher *hasher, const void *input, size_t length) {
    cc_vhasher *vhasher = (cc_vhasher *) hasher;
    const cc_uint8 *data = (const uint8_t *) input;
    vhasher->enc = cc_hash_simd_lower_add(vhasher->enc, length);
    if (length <= 8) {
        cc_hash_small_data value = cc_hash_load_small_data(data, length);
        cc_hash_digest_hashvec(vhasher, cc_hash_simd_u64x2(value.data[0], value.data[1]));
    } else {
        if (length > 32) {
            if (length > 64) {
#  if PROJECT_CL_HASH_HAS_WIDE_SIMD
                if (length > 128 && cc_hash_wide_vectorized_digest(vhasher, data, length)) {
                    return;
                }
#  endif
                cc_hash_vectorized_digest(vhasher, data, length);
            } else {
                cc_hashvec h0 = cc_hash_simd_uload(&data[0]);
                cc_hashvec h1 = cc_hash_simd_uload(&data[16]);
                cc_hashvec t0 = cc_hash_simd_uload(&data[length - 32]);
                cc_hashvec t1 = cc_hash_simd_uload(&data[length - 16]);
                cc_hash_digest_hashvec2(vhasher, h0, h1);
                cc_hash_digest_hashvec2(vhasher, t0, t1);
            }
        } else {
            if (length > 16) {
                cc_hashvec x = cc_hash_simd_uload(&data[0]);
                cc_hashvec y = cc_hash_simd_uload(&data[length - 16]);
                cc_hash_digest_hashvec2(vhasher, x, y);
            } else {
                uint64_t x, y;
                PROJECT_CL_HASH_GENERIC_LOAD(&x, &data[0]);
                PROJECT_CL_HASH_GENERIC_LOAD(&y, &data[length - 8]);
                cc_hash_digest_hashvec(vhasher, cc_hash_simd_u64x2(x, y));
            }
        }
    }
}

#else
/* emulate uint128 for internal use */
typedef struct {
  union {
    _Alignas(16) cc_uint64 data[2];
#ifdef __SIZEOF_INT128__
    __extension__ unsigned __int128 whole_data;
#endif
  };
} cc_impl_uint128;

#ifndef __SIZEOF_INT128__
static inline cc_uint64 *cc_internal_uin128_low(cc_impl_uint128 *x) {
  return &x->data[PROJECT_CL_IS_BIG_ENDIAN()];
}

static inline cc_uint64 *cc_internal_uin128_high(cc_impl_uint128 *x) {
  return &x->data[!PROJECT_CL_IS_BIG_ENDIAN()];
}

static inline void cc_internal_full_mul(cc_uint64 op1, cc_uint64 op2,
                                        cc_uint64 *hi, cc_uint64 *lo) {
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
  cc_internal_full_mul(*cc_internal_uin128_low(&a), *cc_internal_uin128_low(&b),
                       &hi, &lo);
  hi += (*cc_internal_uin128_high(&a)) * (*cc_internal_uin128_low(&b));
  hi += (*cc_internal_uin128_high(&b)) * (*cc_internal_uin128_low(&a));
  *cc_internal_uin128_high(&result) = hi;
  *cc_internal_uin128_low(&result) = lo;
#endif
  return result;
}

#endif
