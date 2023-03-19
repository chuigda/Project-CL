#ifndef PROJECT_CL_HASH_SSSE3_H
#define PROJECT_CL_HASH_SSSE3_H
#ifdef __SSSE3__

#include <immintrin.h>
#include <cc_defs.h>
#include "cc_hash/common.h"
#include "cc_impl.h"

#define PROJECT_CL_HASH_HAS_BASIC_SIMD     1
typedef __m128i cc_hashvec;

inline static cc_size
cc_hash_simd_lanes(cc_hashvec ignored) {
    PROJECT_CL_UNUSED(ignored);
    return 16;
}

inline static cc_hashvec
cc_hash_simd_broadcast(__m128i data) {
    return data;
}

inline static __m128i
cc_hash_simd_downcast(cc_hashvec data, cc_size idx) {
    PROJECT_CL_UNUSED(idx);
    return data;
}

inline static cc_hashvec
cc_hash_simd_encode(cc_hashvec x, cc_hashvec y) {
    return _mm_aesenc_si128(x, y);
}

inline static cc_hashvec
cc_hash_simd_decode(cc_hashvec x, cc_hashvec y) {
    return _mm_aesdec_si128(x, y);
}

inline static cc_hashvec
cc_hash_simd_add64(cc_hashvec x, cc_hashvec y) {
    return _mm_add_epi64(x, y);
}

inline static cc_hashvec
cc_hash_simd_shuffle(cc_hashvec x, cc_hashvec table) {
    return _mm_shuffle_epi8(x, table);
}

inline static cc_hashvec
cc_hash_simd_shuffle_mask(void) {
    return _mm_load_si128((const __m128i *)&PROJECT_CL_HASH_SHUFFLE_TABLE[0]);
}

inline static cc_hashvec
cc_hash_simd_shuffle_add(cc_hashvec x, cc_hashvec y) {
    return cc_hash_simd_add64(cc_hash_simd_add64(x, cc_hash_simd_shuffle_mask()), y);
}

inline static cc_hashvec
cc_hash_simd_uload(const void *target) {
    return _mm_loadu_si128((const cc_hashvec *)target);
}

inline static cc_hashvec
cc_hash_simd_u64x2(cc_uint64 a, cc_uint64 b) {
    return _mm_set_epi64x((cc_int64)a, (cc_int64)b);
}

inline static cc_hashvec
cc_hash_simd_lower_add(cc_hashvec x, cc_uint64 info) {
    cc_hashvec delta = cc_hash_simd_u64x2(0, info);
    return cc_hash_simd_add64(x, delta);
}


inline static cc_uint64
cc_hash_simd_lower_half(cc_hashvec x) {
    return (cc_uint64)_mm_set_epi64x(x, 0);
}

inline static cc_hashvec
cc_hash_simd_xor(cc_hashvec x, cc_hashvec y) {
    return _mm_xor_si128(x, y);
}

#endif
#endif //PROJECT_CL_HASH_SSSE3_H
