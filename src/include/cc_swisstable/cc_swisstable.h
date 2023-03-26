#ifndef PROJECT_CL_SWISSTABLE_H
#define PROJECT_CL_SWISSTABLE_H

#if defined(__SSE2__)
#include "cc_swisstable/sse2.h"
#elif defined(__ARM_NEON)
#include "cc_swisstable/asimd.h"
#else
#include "cc_swisstable/generic.h"
#endif

static inline
_Bool cc_st_ctrl_is_full(cc_st_ctrl ctrl) {
    return (ctrl & 0x80U) == 0;
}

static inline
_Bool cc_st_ctrl_special_is_empty(cc_st_ctrl ctrl) {
    return (ctrl & 0x01U) != 0;
}

static inline
cc_size cc_st_level1_hash(cc_st_hash hash) {
    return (cc_size) (hash);
}

static inline
cc_st_ctrl cc_st_level2_hash(cc_st_hash hash) {
    static const cc_size HASH_LENGTH =
            sizeof(cc_size) < sizeof(cc_st_hash) ? sizeof(cc_size) : sizeof(cc_st_hash);
    return (cc_st_ctrl) ((hash >> (HASH_LENGTH * 8U - 7)) & 0x7FU);
}

typedef struct {
    cc_size position;
    cc_size stride;
} cc_st_probe_seq;

static inline
cc_st_probe_seq cc_st_probe_seq_create(cc_size position, cc_size stride) {
    cc_st_probe_seq seq;
    seq.position = position;
    seq.stride = stride;
    return seq;
}

static inline
void cc_st_probe_seq_move_next(cc_st_probe_seq *seq, cc_size bucket_mask) {
    seq->stride += sizeof(cc_st_group);
    seq->position += seq->stride;
    seq->position &= bucket_mask;
}

static inline
cc_size cc_st_capacity_to_buckets(cc_size cap) {
    if (cap < 8) {
        return (cap < 4) ? 4 : 8;
    }
    return cc_next_pow2_cc_size(cap * 8);
}

static inline
cc_size cc_st_bucket_mask_to_capacity(cc_size bucket_mask) {
    if (bucket_mask < 8) {
        return bucket_mask;
    } else {
        return (bucket_mask + 1) / 8 * 7;
    }
}

#define CC_ST_DUPLICATE_2(X) X, X
#define CC_ST_DUPLICATE_4(X) CC_ST_DUPLICATE_2(X), CC_ST_DUPLICATE_2(X)
#define CC_ST_DUPLICATE_8(X) CC_ST_DUPLICATE_4(X), CC_ST_DUPLICATE_4(X)
#define CC_ST_DUPLICATE_16(X) CC_ST_DUPLICATE_8(X), CC_ST_DUPLICATE_8(X)
#define CC_ST_DUPLICATE_32(X) CC_ST_DUPLICATE_16(X), CC_ST_DUPLICATE_16(X)
#define CC_ST_DUPLICATE_64(X) CC_ST_DUPLICATE_32(X), CC_ST_DUPLICATE_32(X)
#define CC_ST_DUPLICATE_128(X) CC_ST_DUPLICATE_64(X), CC_ST_DUPLICATE_64(X)

static const cc_st_ctrl CC_ST_CONST_EMPTY_GROUP[128] = {
        CC_ST_DUPLICATE_128(CC_ST_EMPTY)
};

static inline
void cc_st_table_layout(
        cc_size elem_size,
        cc_size elem_alignment,
        cc_size num_of_buckets,
        cc_size *offset,
        cc_size *size
) {
    *offset = (num_of_buckets * elem_size + elem_alignment - 1)
              & ~(elem_alignment - 1);
    *size = *offset + num_of_buckets + sizeof(cc_st_group);
}

typedef struct {
    cc_st_ctrl *ptr;
    cc_size element_size;
} cc_st_bucket;

static inline
cc_st_bucket cc_st_bucket_create(cc_st_ctrl *base, cc_size index, cc_size element_size) {
    cc_st_bucket bucket;
    bucket.ptr = base - index * element_size;
    bucket.element_size = element_size;
    return bucket;
}

static inline
cc_st_bucket cc_st_bucket_move(cc_st_bucket from, cc_size offset) {
    cc_st_bucket bucket;
    bucket.ptr = from.ptr - offset * from.element_size;
    bucket.element_size = from.element_size;
    return bucket;
}

static inline
void *cc_st_bucket_get(cc_st_bucket bucket) {
    return bucket.ptr - bucket.element_size;
}

typedef struct {
    cc_size bucket_mask;
    cc_size element_size;
    cc_size element_alignment;
    cc_st_ctrl *ctrl;
    cc_size growth_left;
    cc_size items;
} cc_swisstable;

#endif // PROJECT_CL_SWISSTABLE_H
