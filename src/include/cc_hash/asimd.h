#ifndef PROJECT_CL_HASH_ASIMD_H
#define PROJECT_CL_HASH_ASIMD_H
#ifdef __ARM_NEON

#include <arm_neon.h>
#include <cc_defs.h>
#include "common.h"

#define PROJECT_CL_HASH_HAS_BASIC_SIMD     1
typedef uint8x16_t cc_hashvec;

inline static cc_size
cc_hash_simd_lanes(cc_hashvec ignored) {
    return 16;
}

inline static cc_hashvec
cc_hash_simd_broadcast(uint8x16_t data) {
    return data;
}

inline static uint8x16_t
cc_hash_simd_downcast(cc_hashvec data, cc_size size) {
    return data;
}

inline static cc_hashvec
cc_hash_simd_encode(cc_hashvec x, cc_hashvec y) {
    return vaesmcq_u8(vaeseq_u8(x, vdupq_n_u8(0))) ^ y;
}

inline static cc_hashvec
cc_hash_simd_decode(cc_hashvec x, cc_hashvec y) {
    return vaesimcq_u8(vaesdq_u8(x, vdupq_n_u8(0))) ^ y;
}

inline static cc_hashvec
cc_hash_simd_add64(cc_hashvec x, cc_hashvec y) {
    return vreinterpretq_u8_u64(
            vaddq_u64(vreinterpretq_u64_u8(x), vreinterpretq_u64_u8(y)));
}

inline static cc_hashvec
cc_hash_simd_shuffle(cc_hashvec x, cc_hashvec table) {
    return vqtbl1q_u8(x, table);
}

inline static cc_hashvec
cc_hash_simd_shuffle_mask() {
    return vld1q_u8 (&CC_HASH_SHUFFLE_TABLE[0]);
}

inline static cc_hashvec
cc_hash_simd_shuffle_add(cc_hashvec x, cc_hashvec y) {
    return cc_hash_simd_add64(cc_hash_simd_add64(x, cc_hash_simd_shuffle_mask()), y);
}

inline static cc_hashvec
cc_hash_simd_uload(const void *target) {
    return vld1q_u8 ((const cc_uint8 *)target);
}

inline static cc_hashvec
cc_hash_simd_u64x2(cc_uint64 a, cc_uint64 b) {
    uint64x2_t x = {a, b};
    return vreinterpretq_u8_u64(x);
}

inline static cc_hashvec
cc_hash_simd_lower_add(cc_hashvec x, cc_uint64 info) {
    cc_hashvec delta = cc_hash_simd_u64x2(info, 0);
    return cc_hash_simd_add64(x, delta);
}


inline static cc_uint64
cc_hash_simd_lower_half(cc_hashvec x) {
    return vreinterpretq_u64_u8(x)[0];
}

#endif
#endif //PROJECT_CL_HASH_ASIMD_H
