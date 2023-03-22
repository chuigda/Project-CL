#ifndef PROJECT_CL_HASH_VAES_H
#define PROJECT_CL_HASH_VAES_H
#ifdef __VAES__

#include <immintrin.h>
#include <cc_defs.h>
#include "cc_hash/common.h"
#include "cc_impl.h"

#define PROJECT_CL_HASH_HAS_WIDE_SIMD     1
typedef __m256i cc_wide_hashvec;

inline static cc_size
cc_hash_wide_simd_lanes(cc_wide_hashvec ignored) {
    PROJECT_CL_UNUSED(ignored);
    return 32;
}

inline static cc_wide_hashvec
cc_hash_wide_simd_broadcast(__m128i data) {
    return _mm256_broadcastsi128_si256(data);
}

inline static __m128i
cc_hash_wide_simd_downcast(cc_wide_hashvec data, cc_size idx) {
    if (idx == 0)
        return _mm256_castsi256_si128(data);
    else
        return _mm256_extracti128_si256(data, 1);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_encode(cc_wide_hashvec x, cc_wide_hashvec y) {
    return _mm256_aesenc_epi128(x, y);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_add64(cc_wide_hashvec x, cc_wide_hashvec y) {
    return _mm256_add_epi64(x, y);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_shuffle(cc_wide_hashvec x, cc_wide_hashvec table) {
    return _mm256_shuffle_epi8(x, table);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_shuffle_mask(void) {
    __m128i mask = _mm_load_si128(
        (const __m128i *)&PROJECT_CL_HASH_SHUFFLE_TABLE[0]
    );
    return _mm256_broadcastsi128_si256(mask);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_shuffle_add(cc_wide_hashvec x, cc_wide_hashvec y) {
    return cc_hash_wide_simd_add64(cc_hash_wide_simd_shuffle(
        x,
        cc_hash_wide_simd_shuffle_mask()
    ), y);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_uload(const void *target) {
    return _mm256_loadu_si256((const cc_wide_hashvec *)target);
}
#endif
#endif //PROJECT_CL_HASH_VAES_H
