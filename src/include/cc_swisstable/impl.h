#ifndef PROJECT_CL_SWISSTABLE_IMPL_H
#define PROJECT_CL_SWISSTABLE_IMPL_H

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

#ifndef CC_SWISSTABLE_STACK_ALLOC_THRESHOLD
#define CC_SWISSTABLE_STACK_ALLOC_THRESHOLD 64
#endif

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_ctrl_is_full(cc_st_ctrl ctrl) {
    return (ctrl & 0x80U) == 0;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_ctrl_special_is_empty(cc_st_ctrl ctrl) {
    return (ctrl & 0x01U) != 0;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_size cc_st_level1_hash(cc_st_hash hash) {
    return (cc_size) (hash);
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_ctrl cc_st_level2_hash(cc_st_hash hash) {
    static const cc_size HASH_LENGTH = sizeof(cc_size) < sizeof(cc_st_hash)
                                       ? sizeof(cc_size)
                                       : sizeof(cc_st_hash);
    return (cc_st_ctrl) ((hash >> (HASH_LENGTH * 8U - 7)) & 0x7FU);
}

typedef struct {
    cc_size position;
    cc_size stride;
} cc_st_probe_seq;

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_probe_seq cc_st_probe_seq_create(cc_size position,
                                                     cc_size stride) {
    cc_st_probe_seq seq;
    seq.position = position;
    seq.stride = stride;
    return seq;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void cc_st_probe_seq_move_next(cc_st_probe_seq *seq,
                                             cc_size bucket_mask) {
    seq->stride += sizeof(cc_st_group);
    seq->position += seq->stride;
    seq->position &= bucket_mask;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_size cc_st_capacity_to_buckets(cc_size cap) {
    if (cap < 8) {
        return (cap < 4) ? 4 : 8;
    }
    return cc_next_pow2(cap * 8);
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_size cc_st_bucket_mask_to_capacity(cc_size bucket_mask) {
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

static const _Alignas(cc_st_group) cc_st_ctrl CC_ST_CONST_EMPTY_GROUP[64] = {
        CC_ST_DUPLICATE_64(CC_ST_EMPTY)};

_Static_assert(
        sizeof(CC_ST_CONST_EMPTY_GROUP) >= sizeof(cc_st_group),
        "CC_ST_CONST_EMPTY_GROUP should be at least as large as cc_st_group");

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void cc_st_table_layout_unchecked(cc_size elem_size,
                                                cc_size elem_alignment,
                                                cc_size num_of_buckets,
                                                cc_size *offset,
                                                cc_size *size) {
    const cc_size alignment = elem_alignment > sizeof(cc_st_group)
                              ? elem_alignment
                              : sizeof(cc_st_group);
    *offset = (num_of_buckets * elem_size + alignment - 1) & ~(alignment - 1);
    *size = *offset + num_of_buckets + sizeof(cc_st_group);
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_table_layout_checked(cc_size elem_size,
                                               cc_size elem_alignment,
                                               cc_size num_of_buckets,
                                               cc_size *offset, cc_size *size) {
    const cc_size alignment = elem_alignment > sizeof(cc_st_group)
                              ? elem_alignment
                              : sizeof(cc_st_group);
    cc_st_safe_size without_padding =
            cc_st_safe_size_mul(cc_st_safe_size_create(num_of_buckets),
                                cc_st_safe_size_create(elem_size));
    cc_st_safe_size padded = cc_st_safe_size_add(
            without_padding, cc_st_safe_size_create(alignment - 1));
    _Bool padded_valid = cc_st_safe_size_is_valid(padded);
    *offset = ((cc_size) padded) & ~(alignment - 1);
    cc_st_safe_size safe_size = cc_st_safe_size_add(
            cc_st_safe_size_create(*offset), cc_st_safe_size_create(num_of_buckets));
    safe_size = cc_st_safe_size_add(safe_size,
                                    cc_st_safe_size_create(sizeof(cc_st_group)));
    *size = (cc_size) safe_size;
    return padded_valid && cc_st_safe_size_is_valid(safe_size);
}

typedef struct {
    cc_st_ctrl *ptr;
    cc_size element_size;
} cc_st_bucket;

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_bucket_is_valid(const cc_st_bucket *bucket) {
    return bucket->ptr != NULL;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_bucket cc_st_bucket_create(cc_st_ctrl *base, cc_size index,
                                               cc_size element_size) {
    cc_st_bucket bucket;
    bucket.ptr = base ? base - index * element_size : NULL;
    bucket.element_size = element_size;
    return bucket;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_bucket cc_st_bucket_move(const cc_st_bucket *from,
                                             cc_size offset) {
    cc_st_bucket bucket;
    bucket.ptr = from->ptr - offset * from->element_size;
    bucket.element_size = from->element_size;
    return bucket;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void *cc_st_bucket_get(const cc_st_bucket *bucket) {
    return bucket->ptr - bucket->element_size;
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

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_is_valid(const cc_swisstable *table) {
    return ((cc_st_ctrl *) (table->ctrl)) != NULL;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_swisstable cc_st_create_invalid(cc_size element_size,
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

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_swisstable cc_st_create_empty(cc_size element_size,
                                               cc_size element_alignment) {
    cc_swisstable table;
    table.bucket_mask = 0;
    table.element_size = element_size;
    table.element_alignment = element_alignment;
    table.ctrl = (cc_st_ctrl *) &CC_ST_CONST_EMPTY_GROUP[0];
    table.growth_left = 0;
    table.items = 0;
    return table;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_swisstable
cc_st_create_with_uninitialized(cc_size element_size, cc_size element_alignment,
                                cc_size buckets) {
    const cc_size alignment = element_alignment > sizeof(cc_st_group)
                              ? element_alignment
                              : sizeof(cc_st_group);
    cc_swisstable table = cc_st_create_empty(element_size, element_alignment);
    cc_size offset;
    cc_size size;
    if (CC_UNLIKELY(!cc_st_table_layout_checked(element_size, element_alignment,
                                                buckets, &offset, &size))) {
        return cc_st_create_invalid(element_size, element_alignment);
    }
    size += (-size) & (alignment - 1);
    cc_st_ctrl *address = (cc_st_ctrl *) cc_alloc2(size, alignment);
    if (CC_UNLIKELY(address == NULL)) {
        return cc_st_create_invalid(element_size, element_alignment);
    }
    table.bucket_mask = buckets - 1;
    table.ctrl = address + offset;
    table.growth_left = cc_st_bucket_mask_to_capacity(table.bucket_mask);
    return table;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_swisstable
cc_st_create_with_capacity(cc_size element_size, cc_size element_alignment,
                           cc_size capacity) {
    if (capacity == 0) {
        return cc_st_create_empty(element_size, element_alignment);
    }
    cc_st_safe_size safe_cap = cc_st_safe_size_create(capacity);
    cc_st_safe_size multiplied =
            cc_st_safe_size_mul(safe_cap, (cc_st_safe_size) 8);
    if (!cc_st_safe_size_is_valid(multiplied)) {
        return cc_st_create_invalid(element_size, element_alignment);
    }
    cc_size num_of_buckets = cc_st_capacity_to_buckets(capacity);
    cc_swisstable table = cc_st_create_with_uninitialized(
            element_size, element_alignment, num_of_buckets);
    if (cc_st_is_valid(&table)) {
        cc_memset(table.ctrl, CC_ST_EMPTY, num_of_buckets + sizeof (cc_st_group));
    }
    return table;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_is_empty_singleton(const cc_swisstable *table) {
    return ((cc_st_ctrl *) (table->ctrl)) ==
           (cc_st_ctrl *) &CC_ST_CONST_EMPTY_GROUP[0];
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void cc_st_destroy(cc_swisstable *table) {
    if (!cc_st_is_empty_singleton(table) && cc_st_is_valid(table)) {
        cc_size offset;
        cc_size size;
        cc_st_table_layout_unchecked(table->element_size, table->element_alignment,
                                     table->bucket_mask + 1, &offset, &size);
        cc_free(((cc_st_ctrl *) (table->ctrl)) - offset);
    }
    table->ctrl = NULL;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_size cc_st_num_of_buckets(const cc_swisstable *table) {
    return table->bucket_mask + 1;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_size cc_st_num_of_ctrl_bytes(const cc_swisstable *table) {
    return cc_st_num_of_buckets(table) + sizeof(cc_st_group);
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void *cc_st_data_end(const cc_swisstable *table) {
    return ((cc_st_ctrl *) (table->ctrl));
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void *cc_st_data_begin(const cc_swisstable *table) {
    return ((cc_st_ctrl *) (table->ctrl)) - cc_st_num_of_buckets(table);
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_bucket cc_st_bucket_at(const cc_swisstable *table,
                                           cc_size index) {
    return cc_st_bucket_create(((cc_st_ctrl *) (table->ctrl)), index,
                               table->element_size);
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void cc_st_set_ctrl(cc_swisstable *table, cc_size index,
                                  cc_st_ctrl value) {
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

    cc_size index2 = ((index - sizeof(cc_st_group)) & table->bucket_mask) +
                     sizeof(cc_st_group);
    ((cc_st_ctrl *) (table->ctrl))[index] = value;
    ((cc_st_ctrl *) (table->ctrl))[index2] = value;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_probe_seq
cc_st_probe_seq_create_from_table(const cc_swisstable *table, cc_st_hash hash) {
    return cc_st_probe_seq_create(cc_st_level1_hash(hash) & table->bucket_mask,
                                  0);
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_size cc_st_proper_insert_position(const cc_swisstable *table,
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
    if (CC_UNLIKELY(cc_st_ctrl_is_full(((cc_st_ctrl *) (table->ctrl))[index]))) {
        return cc_st_bitmask_lowest_nz(cc_st_group_mask_empty_or_deleted(
                cc_st_load_group_aligned(((cc_st_ctrl *) (table->ctrl)))));
    }
    return index;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_size cc_st_find_insert_slot(const cc_swisstable *table,
                                             cc_st_hash hash) {
    cc_st_probe_seq seq = cc_st_probe_seq_create_from_table(table, hash);
    while (1) {
        cc_st_group group =
                cc_st_load_group(&((cc_st_ctrl *) (table->ctrl))[seq.position]);
        cc_st_bitmask empty_slot = cc_st_group_mask_empty_or_deleted(group);
        if (cc_st_bitmask_any(empty_slot)) {
            size_t result = (seq.position + cc_st_bitmask_lowest_nz(empty_slot)) &
                            table->bucket_mask;
            return cc_st_proper_insert_position(table, result);
        }
        cc_st_probe_seq_move_next(&seq, table->bucket_mask);
    }
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_bucket cc_st_find_with_hash(const cc_swisstable *table,
                                                cc_st_hash hash, void *key,
                                                cc_pred2 eq) {
    cc_st_ctrl h2_hash = cc_st_level2_hash(hash);
    cc_st_probe_seq seq = cc_st_probe_seq_create_from_table(table, hash);
    while (1) {
        cc_st_group group =
                cc_st_load_group(&((cc_st_ctrl *) (table->ctrl))[seq.position]);
        cc_st_bitmask_iter iter = cc_st_group_match_ctrl(group, h2_hash);
        while (cc_st_bitmask_iter_nonzero(&iter)) {
            cc_size bit = cc_st_bitmask_iter_get_lowest(&iter);
            cc_size index = (seq.position + bit) & table->bucket_mask;
            cc_st_bucket bucket = cc_st_bucket_at(table, index);
            if (CC_LIKELY(eq(cc_st_bucket_get(&bucket), key))) {
                return bucket;
            }
            cc_st_bitmask_iter_rm_lowest(&iter);
        }

        if (CC_LIKELY(cc_st_bitmask_any(cc_st_group_mask_empty(group)))) {
            return cc_st_bucket_create(NULL, 0, table->element_size);
        }

        cc_st_probe_seq_move_next(&seq, table->bucket_mask);
    }
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void cc_st_set_ctrl_h2(cc_swisstable *table, cc_size index,
                                     cc_st_hash hash) {
    cc_st_set_ctrl(table, index, cc_st_level2_hash(hash));
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_ctrl cc_st_replace_ctrl_h2(cc_swisstable *table,
                                               cc_size index, cc_st_hash hash) {
    cc_st_ctrl old = ((cc_st_ctrl *) (table->ctrl))[index];
    cc_st_set_ctrl_h2(table, index, hash);
    return old;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_is_bucket_full(const cc_swisstable *table,
                                         cc_size index) {
    return cc_st_ctrl_is_full(((cc_st_ctrl *) (table->ctrl))[index]);
}

typedef struct {
    size_t index;
    cc_st_ctrl prev_ctrl;
} cc_st_slot;

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_slot cc_st_prepare_insert_slot(cc_swisstable *table,
                                                   cc_st_hash hash) {
    cc_st_slot slot;
    slot.index = cc_st_find_insert_slot(table, hash);
    slot.prev_ctrl = cc_st_replace_ctrl_h2(table, slot.index, hash);
    return slot;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void cc_st_prepare_rehash_inplace(cc_swisstable *table) {
    // convert full to deleted, deleted to empty s.t. we can use
    // deleted as an indicator for rehash
    for (cc_size i = 0; i < cc_st_num_of_buckets(table);
         i += sizeof(cc_st_group)) {
        cc_st_group group =
                cc_st_load_group_aligned(&((cc_st_ctrl *) (table->ctrl))[i]);
        cc_st_group converted =
                cc_st_group_convert_special_to_empty_and_full_to_deleted(group);
        cc_st_store_group_aligned(&((cc_st_ctrl *) (table->ctrl))[i], converted);
    }

    // handle the cases when table size is smaller than group size
    if (cc_st_num_of_buckets(table) < sizeof(cc_st_group)) {
        cc_memcpy(&((cc_st_ctrl *) (table->ctrl))[sizeof(cc_st_group)],
                  &((cc_st_ctrl *) (table->ctrl))[0], cc_st_num_of_buckets(table));
    } else {
#if defined(__has_builtin) && __has_builtin(__builtin_memcpy_inline)
        __builtin_memcpy_inline(
            &((cc_st_ctrl *)(table->ctrl))[cc_st_num_of_buckets(table)],
            &((cc_st_ctrl *)(table->ctrl))[0], sizeof(cc_st_group));
#elif defined(__has_builtin) && __has_builtin(__builtin_memcpy)
        __builtin_memcpy(
            &((cc_st_ctrl *)(table->ctrl))[cc_st_num_of_buckets(table)],
            &((cc_st_ctrl *)(table->ctrl))[0], sizeof(cc_st_group));
#else
        for (cc_size i = 0; i < sizeof(cc_st_group); i++) {
            ((cc_st_ctrl *) (table->ctrl))[cc_st_num_of_buckets(table) + i] =
                    ((cc_st_ctrl *) (table->ctrl))[i];
        }
#endif
    }
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline void cc_st_record_insertion_at(cc_swisstable *table,
                                             cc_size index,
                                             cc_st_ctrl prev_ctrl,
                                             cc_st_hash hash) {
    table->growth_left -= cc_st_ctrl_special_is_empty(prev_ctrl) ? 1 : 0;
    cc_st_set_ctrl_h2(table, index, hash);
    table->items++;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_is_in_same_group(const cc_swisstable *table,
                                           cc_size index, cc_size new_index,
                                           cc_st_hash hash) {
    cc_size probe_position =
            cc_st_probe_seq_create_from_table(table, hash).position;
    cc_size positions[2] = {
            (index - probe_position) & table->bucket_mask / sizeof(cc_st_group),
            (new_index - probe_position) & table->bucket_mask / sizeof(cc_st_group),
    };
    return positions[0] == positions[1];
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_swisstable cc_st_prepare_resize(const cc_swisstable *table,
                                                 cc_size capacity) {
    cc_swisstable new_table = cc_st_create_with_capacity(
            table->element_size, table->element_alignment, capacity);
    if (CC_LIKELY(cc_st_is_valid(&new_table))) {
        new_table.growth_left -= table->items;
        new_table.items += table->items;
    }
    return new_table;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_rehash_inplace(cc_swisstable *table,
                                         cc_swisstable_hasher hasher) {
    void *swap_buffer = NULL;
    _Bool swap_buffer_is_on_heap = 0;
#if defined(__has_builtin) && __has_builtin(__builtin_alloca)
    if (table->element_size <= CC_SWISSTABLE_STACK_ALLOC_THRESHOLD) {
      swap_buffer = __builtin_alloca(table->element_size);
    }
#endif
    if (swap_buffer == NULL) {
        swap_buffer = cc_alloc(table->element_size);
        swap_buffer_is_on_heap = 1;
    }

    if (swap_buffer == NULL) {
        return 0;
    }

    cc_st_prepare_rehash_inplace(table);
    for (cc_size idx = 0; idx < cc_st_num_of_buckets(table); ++idx) {
        if (((cc_st_ctrl *) (table->ctrl))[idx] != CC_ST_DELETED) {
            continue;
        }

        cc_st_bucket bucket = cc_st_bucket_at(table, idx);

        while (1) {
            cc_st_hash hash = (cc_st_hash) hasher.hash(
                    hasher.state, cc_st_bucket_get(&bucket));
            size_t new_idx = cc_st_find_insert_slot(table, hash);
            // Probing works by scanning through all of the control
            // bytes in groups, which may not be aligned to the group
            // size. If both the new and old position fall within the
            // same unaligned group, then there is no benefit in moving
            // it and we can just continue to the next item.
            if (CC_LIKELY(cc_st_is_in_same_group(table, idx, new_idx, hash))) {
                cc_st_set_ctrl_h2(table, idx, hash);
                break; // continue outer loop
            }

            cc_st_bucket new_bucket = cc_st_bucket_at(table, new_idx);
            cc_st_ctrl prev_ctrl = cc_st_replace_ctrl_h2(table, new_idx, hash);

            if (prev_ctrl == CC_ST_EMPTY) {
                cc_st_set_ctrl(table, idx, CC_ST_EMPTY);
                cc_memcpy(cc_st_bucket_get(&new_bucket), cc_st_bucket_get(&bucket),
                          table->element_size);
                break; // continue outer loop
            }

            cc_memcpy(swap_buffer, cc_st_bucket_get(&new_bucket),
                      table->element_size);
            cc_memcpy(cc_st_bucket_get(&new_bucket), cc_st_bucket_get(&bucket),
                      table->element_size);
            cc_memcpy(cc_st_bucket_get(&bucket), swap_buffer, table->element_size);
        }
    }

    table->growth_left =
            cc_st_bucket_mask_to_capacity(table->bucket_mask) - table->items;

    if (swap_buffer_is_on_heap) {
        cc_free(swap_buffer);
    }
    return 1;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_resize(cc_swisstable *table, cc_size new_capacity,
                                 cc_swisstable_hasher hasher) {
    cc_swisstable new_table = cc_st_prepare_resize(table, new_capacity);
    if (CC_UNLIKELY(!cc_st_is_valid(&new_table)))
        return 0;

    for (cc_size i = 0; i < cc_st_num_of_buckets(table); ++i) {
        if (!cc_st_is_bucket_full(table, i))
            continue;

        cc_st_bucket bucket = cc_st_bucket_at(table, i);
        cc_st_hash hash = (cc_st_hash) hasher.hash(
                hasher.state, cc_st_bucket_get(&bucket));

        // We can use a simpler version of insert() here since:
        // - there are no DELETED entries.
        // - we know there is enough space in the table.
        // - all elements are unique.
        cc_st_slot slot = cc_st_prepare_insert_slot(&new_table, hash);
        cc_st_bucket new_bucket = cc_st_bucket_at(&new_table, slot.index);

        cc_memcpy(cc_st_bucket_get(&new_bucket), cc_st_bucket_get(&bucket),
                  table->element_size);
    }

    cc_st_destroy(table);
    *table = new_table;

    return 1;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline _Bool cc_st_reserve_rehash(cc_swisstable *table,
                                         cc_size additional,
                                         cc_swisstable_hasher hasher) {
    cc_st_safe_size checked_new_items = cc_st_safe_size_add(
            cc_st_safe_size_create(table->items), cc_st_safe_size_create(additional));

    if (CC_UNLIKELY(!cc_st_safe_size_is_valid(checked_new_items)))
        return 0;

    cc_size new_items = (cc_size) checked_new_items;
    cc_size full_capacity = cc_st_bucket_mask_to_capacity(table->bucket_mask);

    if (new_items <= full_capacity / 2)
        return cc_st_rehash_inplace(table, hasher);

    if (CC_UNLIKELY(full_capacity == cc_st_max_safe_size() - 1)) {
        return 0;
    }

    cc_size new_capacity =
            new_items > (full_capacity + 1) ? new_items : (full_capacity + 1);
    return cc_st_resize(table, new_capacity, hasher);
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline cc_st_bucket cc_st_insert_at(cc_swisstable *table, cc_size index,
                                           cc_st_hash hash, const void *element,
                                           cc_swisstable_hasher hasher) {
    cc_st_ctrl prev_ctrl = ((cc_st_ctrl *) (table->ctrl))[index];

    // If we reach full load factor:
    //
    //   - When deletion is allowed and the found slot is deleted, then it is
    //     okay to insert.
    //   - Otherwise, it means it is an empty slot and such insertion will
    //     invalidate the load factor constraints. Thus, we need to rehash
    //     the table. There are several cases:
    //
    //       - we may be able to rehash the table in
    //         place if the total item is less than half of the table.
    //       - we may be able to grow the table.
    if (CC_UNLIKELY(table->growth_left == 0 &&
                    cc_st_ctrl_special_is_empty(prev_ctrl))) {
        if (!cc_st_reserve_rehash(table, 1, hasher))
            return cc_st_bucket_create(NULL, 0, table->element_size);
        index = cc_st_find_insert_slot(table, hash);
    }

    cc_st_record_insertion_at(table, index, prev_ctrl, hash);

    cc_st_bucket bucket = cc_st_bucket_at(table, index);
    cc_memcpy(cc_st_bucket_get(&bucket), element, table->element_size);

    return bucket;
}

_Static_assert(sizeof(cc_uint64) >= sizeof(cc_st_bitmask_iter),
               "cc_uint64 must be at least as large as cc_st_bitmask");

CC_ATTRIBUTE_ALWAYS_INLINE
static inline
cc_swisstable_iter cc_st_iter_create(
        cc_st_ctrl *ctrl,
        cc_st_bucket bucket,
        cc_size elems,
        cc_size element_size
) {
    cc_swisstable_iter iter;
    cc_st_bitmask_iter bit_iter;
    bit_iter.mask =
            cc_st_group_mask_full(cc_st_load_group(ctrl)) & cc_st_word_mask();

    iter.bucket_ptr = bucket.ptr;
    iter.element_size = element_size;
    iter.elems = elems;
    iter.next = ctrl + sizeof(cc_st_group);
    iter.opaque = (cc_uint64) bit_iter.mask;
    return iter;
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline
cc_st_bucket cc_st_iter_next_unchecked(
        cc_swisstable_iter *iter
) {
    while (1) {
        cc_st_bitmask_iter bit_iter;
        bit_iter.mask = (cc_st_bitmask) iter->opaque;
        if (cc_st_bitmask_any(bit_iter.mask)) {
            cc_size index = cc_st_bitmask_lowest_nz(bit_iter.mask);
            cc_st_bitmask_iter_rm_lowest(&bit_iter);
            iter->opaque = (cc_uint64) bit_iter.mask;
            return cc_st_bucket_create(
                    (cc_st_ctrl *) iter->bucket_ptr,
                    index,
                    iter->element_size);
        }
        iter->opaque = (cc_uint64)
                cc_st_group_mask_full(cc_st_load_group(iter->next)) & cc_st_word_mask();
        iter->next += sizeof(cc_st_group);
        iter->bucket_ptr = cc_st_bucket_create(
                (cc_st_ctrl *) iter->bucket_ptr, sizeof(cc_st_group), iter->element_size).ptr;
    }
}

CC_ATTRIBUTE_ALWAYS_INLINE
static inline
cc_st_bucket cc_st_iter_next_checked(
        cc_swisstable_iter *iter
) {
    if (CC_UNLIKELY(iter->elems == 0))
        return cc_st_bucket_create(NULL, 0, iter->element_size);
    iter->elems--;
    return cc_st_iter_next_unchecked(iter);
}

#endif // PROJECT_CL_SWISSTABLE_IMPL_H
