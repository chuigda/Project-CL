#ifndef PROJECT_CL_SWISSTABLE_H
#define PROJECT_CL_SWISSTABLE_H

#if defined(__SSE2__)
#include "cc_swisstable/sse2.h"
#elif defined(__ARM_NEON)

#include "cc_swisstable/asimd.h"

#else
#include "cc_swisstable/generic.h"
#endif

#include "cc_swisstable/safe_size.h"
#include <cc_alloc.h>
#include <cc_memory.h>

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

static const _Alignas(cc_st_group)
cc_st_ctrl CC_ST_CONST_EMPTY_GROUP[64] = {
        CC_ST_DUPLICATE_64(CC_ST_EMPTY)
};

_Static_assert(
        sizeof(CC_ST_CONST_EMPTY_GROUP) >= sizeof(cc_st_group),
        "CC_ST_CONST_EMPTY_GROUP should be at least as large as cc_st_group");

static inline
void cc_st_table_layout_unchecked(
        cc_size elem_size,
        cc_size elem_alignment,
        cc_size num_of_buckets,
        cc_size *offset,
        cc_size *size
) {
    const cc_size alignment = elem_alignment > sizeof(cc_st_group)
                              ? elem_alignment : sizeof(cc_st_group);
    *offset = (num_of_buckets * elem_size + alignment - 1)
              & ~(alignment - 1);
    *size = *offset + num_of_buckets + sizeof(cc_st_group);
}

static inline
_Bool cc_st_table_layout_checked(
        cc_size elem_size,
        cc_size elem_alignment,
        cc_size num_of_buckets,
        cc_size *offset,
        cc_size *size
) {
    const cc_size alignment = elem_alignment > sizeof(cc_st_group)
                              ? elem_alignment : sizeof(cc_st_group);
    cc_st_safe_size without_padding = cc_st_safe_size_mul(
            cc_st_safe_size_create(num_of_buckets),
            cc_st_safe_size_create(elem_size)
    );
    cc_st_safe_size padded = cc_st_safe_size_add(
            without_padding,
            cc_st_safe_size_create(alignment - 1)
    );
    _Bool padded_valid = cc_st_safe_size_is_valid(padded);
    *offset = ((cc_size) padded) & ~(alignment - 1);
    cc_st_safe_size safe_size = cc_st_safe_size_add(
            cc_st_safe_size_create(*offset),
            cc_st_safe_size_create(num_of_buckets)
    );
    safe_size = cc_st_safe_size_add(
            safe_size,
            cc_st_safe_size_create(sizeof(cc_st_group))
    );
    *size = (cc_size) safe_size;
    return padded_valid && cc_st_safe_size_is_valid(safe_size);
}

typedef struct {
    cc_st_ctrl *ptr;
    cc_size element_size;
} cc_st_bucket;

static inline
cc_st_bucket cc_st_bucket_create(
        cc_st_ctrl *base, cc_size index, cc_size element_size) {
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

// The heap memory layout for N buckets of size S:
//
//             =========================================
// Fields:     | buckets |  ctrl bytes |     group     |
//             -----------------------------------------
// Size:       |   N*S   |      N      | sizeof(Group) |
//             =========================================
//                       ^
//                       |
//               Store this position in RawTable.
//
// The trailing group part is to make sure we can always load
// a whole group of control bytes.
//
typedef struct {
    cc_size bucket_mask;
    cc_size element_size;
    cc_size element_alignment;
    cc_st_ctrl *ctrl;
    cc_size growth_left;
    cc_size items;
} cc_swisstable;

static inline
_Bool cc_swisstable_is_valid(cc_swisstable *table) {
    return table->ctrl != NULL;
}

static inline
cc_swisstable cc_swisstable_create_invalid(
        cc_size element_size,
        cc_size element_alignment) {
    cc_swisstable table;
    table.bucket_mask = 0;
    table.element_size = element_size;
    table.element_alignment = element_alignment;
    table.ctrl = NULL;
    table.growth_left = 0;
    table.items = 0;
    return table;
}

static inline
cc_swisstable cc_swisstable_create_empty(
        cc_size element_size,
        cc_size element_alignment
) {
    cc_swisstable table;
    table.bucket_mask = 0;
    table.element_size = element_size;
    table.element_alignment = element_alignment;
    table.ctrl = (cc_st_ctrl *) &CC_ST_CONST_EMPTY_GROUP[0];
    table.growth_left = 0;
    table.items = 0;
    return table;
}

static inline
cc_swisstable cc_swisstable_create_with_uninitialized(
        cc_size element_size,
        cc_size element_alignment,
        cc_size buckets
) {
    const cc_size alignment = element_alignment > sizeof(cc_st_group)
                              ? element_alignment : sizeof(cc_st_group);
    cc_swisstable table = cc_swisstable_create_empty(
            element_size,
            element_alignment
    );
    cc_size offset;
    cc_size size;
    if (CC_UNLIKELY(!cc_st_table_layout_checked(
            element_size,
            element_alignment,
            buckets,
            &offset,
            &size
    ))) {
        return cc_swisstable_create_invalid(
                element_size,
                element_alignment
        );
    }
    cc_st_ctrl *address = (cc_st_ctrl *) cc_alloc2(
            size,
            alignment
    );
    if (CC_UNLIKELY(address == NULL)) {
        return cc_swisstable_create_invalid(
                element_size,
                element_alignment
        );
    }
    table.bucket_mask = buckets - 1;
    table.ctrl = address + offset;
    table.growth_left =
            cc_st_bucket_mask_to_capacity(table.bucket_mask);
    return table;
}

static inline
cc_swisstable cc_swisstable_create_with_capacity(
        cc_size element_size,
        cc_size element_alignment,
        cc_size capacity
) {
    if (capacity == 0) {
        return cc_swisstable_create_empty(
                element_size,
                element_alignment
        );
    }
    cc_st_safe_size safe_cap =
            cc_st_safe_size_create(capacity);
    cc_st_safe_size multiplied =
            cc_st_safe_size_mul(safe_cap, (cc_st_safe_size) 8);
    if (!cc_st_safe_size_is_valid(multiplied)) {
        return cc_swisstable_create_invalid(
                element_size,
                element_alignment
        );
    }
    cc_size num_of_buckets = cc_st_capacity_to_buckets(capacity);
    cc_swisstable table = cc_swisstable_create_with_uninitialized(
            element_size,
            element_alignment,
            num_of_buckets
    );
    if (cc_swisstable_is_valid(&table)) {
        cc_memset(table.ctrl, 0, num_of_buckets);
    }
    return table;
}

static inline
_Bool cc_swisstable_is_empty_singleton(cc_swisstable *table) {
    return table->ctrl == (cc_st_ctrl *) &CC_ST_CONST_EMPTY_GROUP[0];
}

static inline
void cc_swisstable_destroy(cc_swisstable *table) {
    if (!cc_swisstable_is_empty_singleton(table) && cc_swisstable_is_valid(table)) {
        cc_size offset;
        cc_size size;
        cc_st_table_layout_unchecked(
                table->element_size,
                table->element_alignment,
                table->bucket_mask + 1,
                &offset,
                &size
        );
        cc_free(table->ctrl - offset);
    }
    table->ctrl = NULL;
}

static inline
cc_size cc_swisstable_num_of_buckets(cc_swisstable *table) {
    return table->bucket_mask + 1;
}

static inline
cc_size cc_swisstable_num_of_ctrl_bytes(cc_swisstable *table) {
    return cc_swisstable_num_of_buckets(table) + sizeof(cc_st_group);
}

static inline
void *cc_swisstable_data_end(cc_swisstable *table) {
    return table->ctrl;
}

static inline
void *cc_swisstable_data_begin(cc_swisstable *table) {
    return table->ctrl - cc_swisstable_num_of_buckets(table);
}

static inline
cc_st_bucket cc_swisstable_bucket_at(cc_swisstable *table, cc_size index) {
    return cc_st_bucket_create(
            table->ctrl,
            index,
            table->element_size
    );
}

static inline
cc_size cc_swisstable_bucket_index(
        cc_swisstable const *table,
        cc_st_bucket const *bucket) {
    return (bucket->ptr - table->ctrl) / table->element_size;
}

static inline
void cc_swisstable_set_ctrl(
        cc_swisstable *table, cc_size index, cc_st_ctrl value) {
    // Replicate the first sizeof(Group) control bytes at the end of
    // the array without using a branch:
    // - If index >= sizeof(Group) then index == index2.
    // - Otherwise index2 == self.bucket_mask + 1 + index.
    //
    // The very last replicated control byte is never actually read because
    // we mask the initial index for unaligned loads, but we write it
    // anyways because it makes the set_ctrl implementation simpler.
    //
    // If there are fewer buckets than sizeof(Group) then this code will
    // replicate the buckets at the end of the trailing group. For example
    // with 2 buckets and a group size of 4, the control bytes will look
    // like this:
    // =============================================
    // |   Real    |            Replicated         |
    // ---------------------------------------------
    // | [A] | [B] | [EMPTY] | [EMPTY] | [A] | [B] |
    // =============================================

    cc_size index2 = ((index - sizeof(cc_st_group)) &
                      table->bucket_mask) + sizeof(cc_st_group);
    table->ctrl[index] = value;
    table->ctrl[index2] = value;
}

static inline
cc_st_probe_seq cc_swisstable_probe_seq(
        cc_swisstable *table,
        cc_st_hash hash
) {
    return cc_st_probe_seq_create(
            cc_st_level1_hash(hash) & table->bucket_mask,
            0);
}

static inline
cc_size cc_swisstable_proper_insert_position(
        cc_swisstable *table,
        cc_size index) {
    // In tables smaller than the group width, trailing control
    // bytes outside the range of the table are filled with
    // EMPTY entries. These will unfortunately trigger a
    // match, but once masked may point to a full bucket that
    // is already occupied. We detect this situation here and
    // perform a second scan starting at the beginning of the
    // table. This second scan is guaranteed to find an empty
    // slot (due to the load factor) before hitting the trailing
    // control bytes (containing EMPTY).
    if (CC_UNLIKELY(cc_st_ctrl_is_full(table->ctrl[index]))) {
        return cc_st_bitmask_lowest_nz(
                cc_st_group_mask_empty_or_deleted(
                        cc_st_load_group_aligned(
                                table->ctrl)));
    }
    return index;
}

static inline
size_t cc_swisstable_find_insert_slot(
        cc_swisstable *table,
        cc_st_hash hash
) {
    cc_st_probe_seq seq = cc_swisstable_probe_seq(table, hash);
    while (1) {
        cc_st_group group = cc_st_load_group(&table->ctrl[seq.position]);
        cc_st_bitmask empty_slot = cc_st_group_mask_empty_or_deleted(group);
        if (cc_st_bitmask_any(empty_slot)) {
            size_t result =
                    (seq.position + cc_st_bitmask_lowest_nz(empty_slot))
                    & table->bucket_mask;
            return cc_swisstable_proper_insert_position(table, result);
        }
        cc_st_probe_seq_move_next(&seq, table->bucket_mask);
    }
}

#endif // PROJECT_CL_SWISSTABLE_H
