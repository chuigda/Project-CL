#ifndef PROJECT_CL_HASH_SVE_H
#define PROJECT_CL_HASH_SVE_H
#if defined(__ARM_FEATURE_AES) && defined(__ARM_FEATURE_SVE2_AES)

#include <arm_neon.h>
#include <arm_sve.h>
#include <cc_defs.h>
#include "cc_hash/common.h"
#include "cc_impl.h"

#define PROJECT_CL_HASH_HAS_WIDE_SIMD     1
typedef svuint8_t cc_wide_hashvec;
static _Alignas(64) cc_uint8 PROJECT_CL_HASH_EXTENDED_SHUFFLE_TABLE[16 * 16] = {
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(0),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(1),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(2),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(3),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(4),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(5),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(6),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(7),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(8),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(9),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(10),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(11),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(12),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(13),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(14),
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(15)
};

inline static cc_size
cc_hash_wide_simd_lanes(cc_wide_hashvec x) {
    return svlen_u8(x);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_broadcast(uint8x16_t data) {
    return svld1rq_u8(svptrue_b8 (), (const cc_uint8 *)(&data));
}

inline static uint8x16_t
cc_hash_wide_simd_downcast(cc_wide_hashvec data, cc_size idx) {
    uint8x16_t result[1];
    svbool_t l = svwhilege_b8_u64 (0, idx * 16);
    svbool_t r = svwhilelt_b8_u64 (0, (idx + 1) * 16);
    svbool_t range = svand_b_z (svptrue_b8 (), l, r);
    svst1_u8 (range, (uint8_t *)(&result[0] - idx), data);
    return result[0];
}

inline static cc_wide_hashvec
cc_hash_wide_simd_encode(cc_wide_hashvec x, cc_wide_hashvec y) {
    return sveor_u8_x(svptrue_b8 (), svaesmc_u8 (svaese_u8 (x, svdup_n_u8(0))), y);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_add64(cc_wide_hashvec x, cc_wide_hashvec y) {
    return svreinterpret_u8_u64(svadd_u64_x(svptrue_b8 (), svreinterpret_u64_u8(x), svreinterpret_u64_u8(y)));
}

inline static cc_wide_hashvec
cc_hash_wide_simd_shuffle(cc_wide_hashvec x, cc_wide_hashvec table) {
    return svtbl_u8 (x, table);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_shuffle_mask(void) {
    return svld1_u8 (svptrue_b8 (), (const cc_uint8 *)&PROJECT_CL_HASH_EXTENDED_SHUFFLE_TABLE[0]);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_shuffle_add(cc_wide_hashvec x, cc_wide_hashvec y) {
    return cc_hash_wide_simd_add64(cc_hash_wide_simd_add64(x, cc_hash_wide_simd_shuffle_mask()), y);
}

inline static cc_wide_hashvec
cc_hash_wide_simd_uload(const void *target) {
    return svld1_u8 (svptrue_b8 (), (const uint8_t *)target);
}
#endif
#endif //PROJECT_CL_HASH_SVE_H
