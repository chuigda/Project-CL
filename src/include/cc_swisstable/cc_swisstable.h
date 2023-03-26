#ifndef PROJECT_CL_SWISSTABLE_H
#define PROJECT_CL_SWISSTABLE_H

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
void cc_st_probe_seq_move_next(cc_st_probe_seq * seq, cc_size bucket_mask) {
    seq->stride += sizeof(cc_st_group);
    seq->position += seq->stride;
    seq->position &= bucket_mask;
}

static inline
cc_size cc_st_capacity_to_buckets(cc_size cap) {
    if (cap < 8) {
        return (cap < 4) ? 4 : 8;
    }
    return cc_next_pow2(cap * 8);
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

#endif // PROJECT_CL_SWISSTABLE_H
