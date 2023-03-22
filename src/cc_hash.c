#include "cc_hash/asimd.h"
#include "cc_hash/common.h"
#include "cc_hash/ssse3.h"
#include "cc_hash/sve.h"
#include "cc_hash/vaes.h"
#include "cc_impl.h"
#include <cc_alloc.h>
#include <cc_hash.h>

/* emulate uint128 for internal use */
typedef struct {
  union {
    _Alignas(16) cc_uint64 data[2];
#ifdef __SIZEOF_INT128__
    __extension__ unsigned __int128 whole_data;
#endif
  };
} cc_hash_uint128;

static inline cc_uint64 *cc_hash_uint128_low(cc_hash_uint128 *x) {
  return &x->data[PROJECT_CL_IS_BIG_ENDIAN()];
}

static inline cc_uint64 *cc_hash_uint128_high(cc_hash_uint128 *x) {
  return &x->data[!PROJECT_CL_IS_BIG_ENDIAN()];
}

#ifndef __SIZEOF_INT128__
static inline void cc_hash_full_mul(cc_uint64 op1,
                                    cc_uint64 op2,
                                    cc_uint64 *hi,
                                    cc_uint64 *lo) {
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

static inline cc_hash_uint128 
cc_hash_uint128_multiply(cc_hash_uint128 a, cc_hash_uint128 b) {
  cc_hash_uint128 result;
#ifdef __SIZEOF_INT128__
  result.whole_data = a.whole_data * b.whole_data;
#else
  cc_uint64 hi, lo;
  cc_hash_full_mul(*cc_hash_uint128_low(&a),
                   *cc_hash_uint128_low(&b), &hi,
                   &lo);
  hi += (*cc_hash_uint128_high(&a)) * (*cc_hash_uint128_low(&b));
  hi += (*cc_hash_uint128_high(&b)) * (*cc_hash_uint128_low(&a));
  *cc_hash_uint128_high(&result) = hi;
  *cc_hash_uint128_low(&result) = lo;
#endif
  return result;
}

static inline cc_hash_uint128 
cc_hash_cast_uint64_to_uint128(cc_uint64 low) {
  cc_hash_uint128 data;
  data.data[PROJECT_CL_IS_BIG_ENDIAN()] = low;
  data.data[!PROJECT_CL_IS_BIG_ENDIAN()] = 0;
  return data;
}

static inline cc_uint64 cc_hash_swap_bytes64(cc_uint64 x) {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap64)
  return __builtin_bswap64(x);
#else
  x = ((x << 8) & 0xFF00FF00FF00FF00ULL) | 
      ((x >> 8) & 0x00FF00FF00FF00FFULL);
  x = ((x << 16) & 0xFFFF0000FFFF0000ULL) |
      ((x >> 16) & 0x0000FFFF0000FFFFULL);
  x = ((x << 32) & 0xFFFFFFFF00000000LL) |
      ((x >> 32) & 0x00000000FFFFFFFFULL);
  return x;
#endif
}

static inline cc_uint32 cc_hash_swap_bytes32(cc_uint32 x) {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap32)
  return __builtin_bswap32(x);
#else
  x = ((x << 8) & 0xFF00FF00U) | ((x >> 8) & 0x00FF00FFU);
  x = ((x << 16) & 0xFFFF0000U) | ((x >> 16) & 0x0000FFFFU);
  return x;
#endif
}

static inline cc_uint16 cc_hash_swap_bytes16(cc_uint16 x) {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap16)
  return __builtin_bswap16(x);
#else
  x = ((x << 8) & 0xFF00U) | ((x >> 8) & 0x00FFU);
  return x;
#endif
}

// load and always make sure that low address is less significant
static inline cc_hash_uint128 cc_hash_load_uint128le(const void *data) {
  cc_hash_uint128 value;
  const cc_uint64 *src = (const cc_uint64 *)data;
  PROJECT_CL_HASH_GENERIC_LOAD(&value.data[0], &src[0]);
  PROJECT_CL_HASH_GENERIC_LOAD(&value.data[1], &src[1]);
  if (PROJECT_CL_IS_BIG_ENDIAN()) {
    cc_uint64 low = cc_hash_swap_bytes64(value.data[0]);
    cc_uint64 high = cc_hash_swap_bytes64(value.data[1]);
    value.data[0] = high;
    value.data[1] = low;
  }
  return value;
}

static inline cc_uint64 cc_hash_load_uint64le(const void *data) {
  cc_uint64 value;
  PROJECT_CL_HASH_GENERIC_LOAD(&value, data);
  if (PROJECT_CL_IS_BIG_ENDIAN()) {
    value = cc_hash_swap_bytes64(value);
  }
  return value;
}

static inline cc_uint32 cc_hash_load_uint32le(const void *data) {
  cc_uint32 value;
  PROJECT_CL_HASH_GENERIC_LOAD(&value, data);
  if (PROJECT_CL_IS_BIG_ENDIAN()) {
    value = cc_hash_swap_bytes32(value);
  }
  return value;
}

static inline cc_uint16 cc_hash_load_uint16le(const void *data) {
  cc_uint16 value;
  PROJECT_CL_HASH_GENERIC_LOAD(&value, data);
  if (PROJECT_CL_IS_BIG_ENDIAN()) {
    value = cc_hash_swap_bytes16(value);
  }
  return value;
}

static inline cc_uint64 cc_hash_rotate_left(cc_uint64 x, cc_uint64 y) {
#if defined(__has_builtin) && __has_builtin(__builtin_rotateleft64)
  return __builtin_rotateleft64(x, y);
#else
  return (x << y) | (x >> (64 - y));
#endif
}

static inline cc_uint64 cc_hash_folded_multiply(cc_uint64 x,
                                                cc_uint64 y) {
  cc_hash_uint128 a = cc_hash_cast_uint64_to_uint128(x);
  cc_hash_uint128 b = cc_hash_cast_uint64_to_uint128(y);
  cc_hash_uint128 c = cc_hash_uint128_multiply(a, b);
  cc_uint64 low = *cc_hash_uint128_low(&c);
  cc_uint64 high = *cc_hash_uint128_high(&c);
  return low ^ high;
}

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

static inline cc_hash_small_data
cc_hash_load_small_data(const void *source, cc_size length) {
  cc_hash_small_data buffer;
  const uint8_t *data = (const uint8_t *)source;
  if (length >= 2) {
    if (length >= 4) {
      buffer.data[0] = 
        (cc_uint64)cc_hash_load_uint32le(&data[0]);
      buffer.data[1] = 
        (cc_uint64)cc_hash_load_uint32le(&data[length - 4]);
    } else {
      cc_uint16 head = cc_hash_load_uint16le(&data[0]);
      cc_uint8 tail = data[length - 1];
      buffer.data[0] = (cc_uint64)head;
      buffer.data[1] = (cc_uint64)tail;
    }
  } else {
    if (length > 0) {
      buffer.data[0] = (cc_uint64)data[0];
      buffer.data[1] = (cc_uint64)data[0];
    } else {
      buffer.data[0] = 0;
      buffer.data[1] = 0;
    }
  }
  return buffer;
}

typedef struct {
  cc_uint64 buffer;
  cc_uint64 pad;
  cc_uint64 extra_keys[2];
} cc_stable_hasher;

static const 
cc_size PROJECT_CL_STABLE_HASH_ROTATE = 23;
static const 
cc_size PROJECT_CL_STABLE_HASH_MULTIPLE = 6364136223846793005;

static inline void cc_hash_stable_digest128(cc_stable_hasher *hasher,
                                            cc_hash_uint128 data) {
  cc_uint64 low = *cc_hash_uint128_low(&data);
  cc_uint64 high = *cc_hash_uint128_high(&data);
  cc_uint64 combined = cc_hash_folded_multiply(
    low ^ hasher->extra_keys[0],
    high ^ hasher->extra_keys[1]
  );
  hasher->buffer = cc_hash_rotate_left(
    (hasher->buffer + hasher->pad) ^ combined,
    PROJECT_CL_STABLE_HASH_ROTATE
  );
}

static inline cc_uint64
cc_finalize_stable_hasher_inplace(cc_hasher_handle hasher) {
  const cc_stable_hasher *shasher = (const cc_stable_hasher *)hasher;
  cc_uint64 rot = shasher->buffer & 63;
  return cc_hash_rotate_left(
      cc_hash_folded_multiply(shasher->buffer, shasher->pad), rot);
}

CC_ATTRIBUTE_EXPORT
cc_uint64 cc_finalize_stable_hasher(cc_hasher_handle hasher) {
  cc_uint64 hash = cc_finalize_stable_hasher_inplace(hasher);
  cc_free(hasher);
  return hash;
}

static inline cc_uint64 
cc_hash_stable_mix_integer(cc_stable_hasher hasher,
                           cc_uint64 x,
                           cc_uint64 y) {
  hasher.buffer = cc_hash_folded_multiply(
    x ^ hasher.buffer,
    PROJECT_CL_STABLE_HASH_MULTIPLE
  );
  hasher.buffer = cc_hash_folded_multiply(
    y ^ hasher.buffer,
    PROJECT_CL_STABLE_HASH_MULTIPLE
  );
  return cc_finalize_stable_hasher_inplace((cc_hasher_handle)&hasher);
}

static inline void 
cc_initialize_stable_hasher_inplace(cc_hasher_handle hasher,
                                    cc_uint64 seed) {
  cc_stable_hasher *shasher = (cc_stable_hasher *)hasher;
  shasher->buffer = PI[0];
  shasher->pad = PI[1];
  shasher->extra_keys[0] = PI[2];
  shasher->extra_keys[1] = PI[3];
  shasher->buffer = cc_hash_folded_multiply(
    seed ^ shasher->buffer,
    PROJECT_CL_STABLE_HASH_MULTIPLE
  );
  cc_uint64 mixed[4] = {
      cc_hash_stable_mix_integer(*shasher, PI2[0], PI2[2]),
      cc_hash_stable_mix_integer(*shasher, PI2[1], PI2[3]),
      cc_hash_stable_mix_integer(*shasher, PI2[2], PI2[1]),
      cc_hash_stable_mix_integer(*shasher, PI2[3], PI2[0]),
  };
  shasher->buffer = mixed[0];
  shasher->pad = mixed[1];
  shasher->extra_keys[0] = mixed[2];
  shasher->extra_keys[1] = mixed[3];
}

CC_ATTRIBUTE_EXPORT
cc_hasher_handle cc_initialize_stable_hasher(cc_uint64 seed) {
  cc_hasher_handle hasher =
      (cc_hasher_handle)cc_alloc2(sizeof(cc_stable_hasher), 16);
  cc_initialize_stable_hasher_inplace(hasher, seed);
  return hasher;
}

CC_ATTRIBUTE_EXPORT
void cc_stable_hasher_digest(cc_hasher_handle hasher, const void *input,
                             cc_size length) {
  cc_stable_hasher *shasher = (cc_stable_hasher *)hasher;
  const cc_uint8 *data = (const cc_uint8 *)input;
  shasher->buffer =
      (shasher->buffer + length) * PROJECT_CL_STABLE_HASH_MULTIPLE;

  if (length > 8) {
    if (length > 16) {
      cc_hash_uint128 tail = cc_hash_load_uint128le(&data[length - 16]);
      cc_hash_stable_digest128(shasher, tail);
      while (length > 16) {
        cc_hash_uint128 head = cc_hash_load_uint128le(&data[0]);
        cc_hash_stable_digest128(shasher, head);
        data += 16;
        length -= 16;
      }
    } else {
      cc_uint64 x = cc_hash_load_uint64le(&data[0]);
      cc_uint64 y = cc_hash_load_uint64le(&data[length - 8]);
      cc_hash_uint128 z;
      *cc_hash_uint128_low(&z) = x;
      *cc_hash_uint128_high(&z) = y;
      cc_hash_stable_digest128(shasher, z);
    }
  } else {
    cc_hash_small_data value = cc_hash_load_small_data(data, length);
    cc_hash_uint128 z;
    *cc_hash_uint128_low(&z) = value.data[0];
    *cc_hash_uint128_high(&z) = value.data[1];
    cc_hash_stable_digest128(shasher, z);
  }
}

#if PROJECT_CL_HASH_HAS_BASIC_SIMD
typedef struct {
  cc_hashvec key;
  cc_hashvec enc;
  cc_hashvec sum;
} cc_simd_hasher;

static inline void cc_simd_hash_digest_hashvec(cc_simd_hasher *hasher,
                                               cc_hashvec state) {
  hasher->enc = cc_hash_simd_encode(hasher->enc, state);
  hasher->sum = cc_hash_simd_shuffle_add(hasher->sum, state);
}

static inline void cc_simd_hash_digest_hashvec2(cc_simd_hasher *hasher,
                                                cc_hashvec x,
                                                cc_hashvec y) {
  cc_simd_hash_digest_hashvec(hasher, x);
  cc_simd_hash_digest_hashvec(hasher, y);
}

#define PROJECT_CL_GENERATE_VECTORIZED_DIGEST(UTYPE, DTYPE, PREFIX)     \
  static inline _Bool PREFIX##_vectorized_digest(                       \
      cc_simd_hasher *hasher, const cc_uint8 *data, cc_size length) {   \
    const UTYPE duplicated_key = PREFIX##_simd_broadcast(hasher->key);  \
    const cc_size num_lanes = PREFIX##_simd_lanes(duplicated_key);      \
    if (4 * num_lanes >= length)                                        \
      return 0;                                                         \
    const UTYPE tail0 =                                                 \
      PREFIX##_simd_uload(&data[length - 4 * num_lanes]);               \
    const UTYPE tail1 =                                                 \
      PREFIX##_simd_uload(&data[length - 3 * num_lanes]);               \
    const UTYPE tail2 =                                                 \
      PREFIX##_simd_uload(&data[length - 2 * num_lanes]);               \
    const UTYPE tail3 =                                                 \
      PREFIX##_simd_uload(&data[length - 1 * num_lanes]);               \
    UTYPE current0 = PREFIX##_simd_encode(duplicated_key, tail0);       \
    UTYPE current1 = PREFIX##_simd_encode(duplicated_key, tail1);       \
    UTYPE current2 = PREFIX##_simd_encode(duplicated_key, tail2);       \
    UTYPE current3 = PREFIX##_simd_encode(duplicated_key, tail3);       \
    UTYPE sum0 = PREFIX##_simd_shuffle_add(                             \
        PREFIX##_simd_add64(duplicated_key, tail0), tail2);             \
    UTYPE sum1 = PREFIX##_simd_shuffle_add(                             \
        PREFIX##_simd_add64(duplicated_key, tail1), tail3);             \
    while (length > 4 * num_lanes) {                                    \
      const UTYPE head0 = PREFIX##_simd_uload(&data[0 * num_lanes]);    \
      const UTYPE head1 = PREFIX##_simd_uload(&data[1 * num_lanes]);    \
      const UTYPE head2 = PREFIX##_simd_uload(&data[2 * num_lanes]);    \
      const UTYPE head3 = PREFIX##_simd_uload(&data[3 * num_lanes]);    \
      current0 = PREFIX##_simd_encode(current0, head0);                 \
      current1 = PREFIX##_simd_encode(current1, head1);                 \
      current2 = PREFIX##_simd_encode(current2, head2);                 \
      current3 = PREFIX##_simd_encode(current3, head3);                 \
      sum0 = PREFIX##_simd_shuffle_add(sum0, head0);                    \
      sum1 = PREFIX##_simd_shuffle_add(sum1, head1);                    \
      sum0 = PREFIX##_simd_shuffle_add(sum0, head2);                    \
      sum1 = PREFIX##_simd_shuffle_add(sum1, head3);                    \
      data += 4 * num_lanes;                                            \
      length -= 4 * num_lanes;                                          \
    }                                                                   \
    const UTYPE encoded0 = PREFIX##_simd_encode(current0, current1);    \
    const UTYPE encoded1 = PREFIX##_simd_encode(current2, current3);    \
    const UTYPE total = PREFIX##_simd_add64(sum0, sum1);                \
    for (cc_size i = 0; i < num_lanes / 16; ++i) {                      \
      DTYPE a = PREFIX##_simd_downcast(encoded0, i);                    \
      DTYPE b = PREFIX##_simd_downcast(encoded1, i);                    \
      DTYPE c = PREFIX##_simd_downcast(total, i);                       \
      cc_simd_hash_digest_hashvec2(hasher, a, b);                       \
      cc_simd_hash_digest_hashvec(hasher, c);                           \
    }                                                                   \
    return 1;                                                           \
  }

PROJECT_CL_GENERATE_VECTORIZED_DIGEST(cc_hashvec, cc_hashvec, cc_hash)

#if PROJECT_CL_HASH_HAS_WIDE_SIMD

PROJECT_CL_GENERATE_VECTORIZED_DIGEST(
  cc_wide_hashvec,
  cc_hashvec,
  cc_hash_wide
)

#endif

static inline cc_uint64
cc_finalize_simd_hasher_inplace(cc_hasher_handle hasher) {
  const cc_simd_hasher *vhasher = (const cc_simd_hasher *)hasher;
  cc_hashvec combined = cc_hash_simd_decode(vhasher->sum, vhasher->enc);
  cc_hashvec result = cc_hash_simd_encode(
      cc_hash_simd_encode(combined, vhasher->key), combined);
  return cc_hash_simd_lower_half(result);
}

static inline cc_uint64
cc_finalize_simd_hasher(cc_hasher_handle hasher) {
  cc_uint64 hash = cc_finalize_simd_hasher_inplace(hasher);
  cc_free(hasher);
  return hash;
}

static inline cc_uint64
cc_hash_simd_mix_integer(cc_simd_hasher hasher,
                         cc_uint64 x, cc_uint64 y) {
  cc_simd_hash_digest_hashvec(&hasher, cc_hash_simd_u64x2(x, 0));
  cc_simd_hash_digest_hashvec(&hasher, cc_hash_simd_u64x2(y, 0));
  return cc_finalize_simd_hasher_inplace((cc_hasher_handle)&hasher);
}

static inline void
cc_initialize_simd_hasher_inplace(cc_hasher_handle hasher,
                                  cc_uint64 seed) {
  cc_simd_hasher *vhasher = (cc_simd_hasher *)hasher;
  vhasher->enc = cc_hash_simd_u64x2(PI[0], PI[1]);
  vhasher->sum = cc_hash_simd_u64x2(PI[2], PI[3]);
  vhasher->key = cc_hash_simd_xor(vhasher->enc, vhasher->sum);
  cc_simd_hash_digest_hashvec(vhasher, cc_hash_simd_u64x2(seed, 0));
  cc_uint64 mixed[4] = {
      cc_hash_simd_mix_integer(*vhasher, PI2[0], PI2[2]),
      cc_hash_simd_mix_integer(*vhasher, PI2[1], PI2[3]),
      cc_hash_simd_mix_integer(*vhasher, PI2[2], PI2[1]),
      cc_hash_simd_mix_integer(*vhasher, PI2[3], PI2[0]),
  };
  vhasher->enc = cc_hash_simd_u64x2(mixed[0], mixed[1]);
  vhasher->sum = cc_hash_simd_u64x2(mixed[2], mixed[3]);
  vhasher->key = cc_hash_simd_xor(vhasher->enc, vhasher->sum);
}

static inline cc_hasher_handle 
cc_initialize_simd_hasher(cc_uint64 seed) {
  cc_hasher_handle hasher = cc_alloc2(sizeof(cc_simd_hasher), 16);
  cc_initialize_simd_hasher_inplace(hasher, seed);
  return hasher;
}

static inline void 
cc_simd_hasher_digest(cc_hasher_handle hasher,
                      const void *input,
                      size_t length) {
  cc_simd_hasher *vhasher = (cc_simd_hasher *)hasher;
  const cc_uint8 *data = (const uint8_t *)input;
  vhasher->enc = cc_hash_simd_lower_add(vhasher->enc, length);
  if (length <= 8) {
    cc_hash_small_data value = cc_hash_load_small_data(data, length);
    cc_simd_hash_digest_hashvec(
        vhasher, cc_hash_simd_u64x2(value.data[0], value.data[1]));
  } else {
    if (length > 32) {
      if (length > 64) {
#if PROJECT_CL_HASH_HAS_WIDE_SIMD
        if (length > 128 &&
            cc_hash_wide_vectorized_digest(vhasher, data, length)) {
          return;
        }
#endif
        cc_hash_vectorized_digest(vhasher, data, length);
      } else {
        cc_hashvec h0 = cc_hash_simd_uload(&data[0]);
        cc_hashvec h1 = cc_hash_simd_uload(&data[16]);
        cc_hashvec t0 = cc_hash_simd_uload(&data[length - 32]);
        cc_hashvec t1 = cc_hash_simd_uload(&data[length - 16]);
        cc_simd_hash_digest_hashvec2(vhasher, h0, h1);
        cc_simd_hash_digest_hashvec2(vhasher, t0, t1);
      }
    } else {
      if (length > 16) {
        cc_hashvec x = cc_hash_simd_uload(&data[0]);
        cc_hashvec y = cc_hash_simd_uload(&data[length - 16]);
        cc_simd_hash_digest_hashvec2(vhasher, x, y);
      } else {
        uint64_t x, y;
        PROJECT_CL_HASH_GENERIC_LOAD(&x, &data[0]);
        PROJECT_CL_HASH_GENERIC_LOAD(&y, &data[length - 8]);
        cc_simd_hash_digest_hashvec(vhasher, cc_hash_simd_u64x2(x, y));
      }
    }
  }
}

#endif

CC_ATTRIBUTE_EXPORT
cc_hasher_handle cc_initialize_unstable_hasher(cc_uint64 seed) {
#if PROJECT_CL_HASH_HAS_BASIC_SIMD
  return cc_initialize_simd_hasher(seed);
#else
  return cc_initialize_stable_hasher(seed);
#endif
}

CC_ATTRIBUTE_EXPORT
cc_uint64 cc_finalize_unstable_hasher(cc_hasher_handle hasher) {
#if PROJECT_CL_HASH_HAS_BASIC_SIMD
  return cc_finalize_simd_hasher(hasher);
#else
  return cc_finalize_stable_hasher(hasher);
#endif
}

CC_ATTRIBUTE_EXPORT
void cc_unstable_hasher_digest(cc_hasher_handle hasher,
                               const void *input,
                               cc_size length) {
#if PROJECT_CL_HASH_HAS_BASIC_SIMD
  cc_simd_hasher_digest(hasher, input, length);
#else
  cc_stable_hasher_digest(hasher, input, length);
#endif
}

CC_ATTRIBUTE_EXPORT
cc_uint64 cc_unstable_hash(const void *input,
                           cc_size length,
                           cc_uint64 seed) {
#if PROJECT_CL_HASH_HAS_BASIC_SIMD
  cc_simd_hasher hasher;
  cc_initialize_simd_hasher_inplace((cc_hasher_handle)&hasher, seed);
  cc_simd_hasher_digest((cc_hasher_handle)&hasher, input, length);
  return cc_finalize_simd_hasher_inplace((cc_hasher_handle)&hasher);
#else
  return cc_stable_hash(input, length, seed);
#endif
}

CC_ATTRIBUTE_EXPORT
cc_uint64 cc_stable_hash(const void *input,
                         cc_size length,
                         cc_uint64 seed) {
  cc_stable_hasher hasher;
  cc_initialize_stable_hasher_inplace((cc_hasher_handle)&hasher, seed);
  cc_stable_hasher_digest((cc_hasher_handle)&hasher, input, length);
  return cc_finalize_stable_hasher_inplace((cc_hasher_handle)&hasher);
}

CC_ATTRIBUTE_EXPORT
_Bool cc_hash_simd_accelerated(void) {
#if PROJECT_CL_HASH_HAS_BASIC_SIMD
  return 1;
#else
  return 0;
#endif
}

CC_ATTRIBUTE_EXPORT
_Bool cc_hash_wide_simd_accelerated(void) {
#if PROJECT_CL_HASH_HAS_WIDE_SIMD
  return 1;
#else
  return 0;
#endif
}
