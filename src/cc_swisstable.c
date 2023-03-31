#include "cc_bits.h"
#include "cc_swisstable/impl.h"
#include <cc_cfg.h>

void * cc_swisstable_insert(cc_swisstable * table,
                            const void * element,
                            cc_swisstable_hasher hasher)
{
    cc_st_hash hash = hasher.hash(hasher.state, element);
    cc_size index = cc_st_find_insert_slot(table, hash);
    cc_st_bucket bucket
        = cc_st_insert_at(table, index, hash, element, hasher);
    if (CC_UNLIKELY(!cc_st_bucket_is_valid(&bucket)))
    {
        return NULL;
    }
    return cc_st_bucket_get(&bucket);
}

cc_swisstable cc_swisstable_empty(cc_size element_size,
                                  cc_size element_alignment)
{
    return cc_st_create_empty(element_size, element_alignment);
}

cc_swisstable cc_swisstable_with_capacity(cc_size element_size,
                                          cc_size element_alignment,
                                          cc_size capacity)
{
    return cc_st_create_with_capacity(element_size,
                                      element_alignment,
                                      capacity);
}

void * cc_swisstable_find(const cc_swisstable * table,
                          void * element,
                          cc_swisstable_hasher hasher,
                          cc_pred2 equal)
{
    cc_st_hash hash = hasher.hash(hasher.state, element);
    cc_st_bucket bucket
        = cc_st_find_with_hash(table, hash, element, equal);
    if (CC_UNLIKELY(!cc_st_bucket_is_valid(&bucket)))
    {
        return NULL;
    }
    return cc_st_bucket_get(&bucket);
}

void cc_swisstable_erase(cc_swisstable * table, void * element)
{
    cc_size index
        = (((cc_st_ctrl *)table->ctrl) - ((cc_st_ctrl *)element))
            / table->element_size
        - 1;
    cc_size index_before
        = (index - sizeof(cc_st_group)) & table->bucket_mask;
    cc_st_bitmask empty_before = cc_st_group_mask_empty(
        cc_st_load_group(&((cc_st_ctrl *)table->ctrl)[index_before]));
    cc_st_bitmask empty_after = cc_st_group_mask_empty(
        cc_st_load_group(&((cc_st_ctrl *)table->ctrl)[index]));
    cc_st_ctrl ctrl;
    if (cc_st_bitmask_lzeros(empty_before)
            + cc_st_bitmask_tzeros(empty_after)
        >= sizeof(cc_st_group))
    {
        ctrl = CC_ST_DELETED;
    }
    else
    {
        table->growth_left++;
        ctrl = CC_ST_EMPTY;
    }
    cc_st_set_ctrl(table, index, ctrl);
    table->items--;
}

void cc_swisstable_destroy(cc_swisstable * table)
{
    cc_st_destroy(table);
}

cc_swisstable_iter cc_swisstable_create_iter(cc_swisstable * table)
{
    return cc_st_iter_create((cc_st_ctrl *)table->ctrl,
                             cc_st_bucket_at(table, 0),
                             table->items,
                             table->element_size);
}

void * cc_swisstable_iter_next(cc_swisstable_iter * iter)
{
    cc_st_bucket bucket = cc_st_iter_next_checked(iter);
    if (CC_UNLIKELY(!cc_st_bucket_is_valid(&bucket)))
    {
        return NULL;
    }
    return cc_st_bucket_get(&bucket);
}

_Bool cc_swisstable_reserve(cc_swisstable * table,
                            cc_size additional,
                            cc_swisstable_hasher hasher)
{
    return cc_st_reserve_rehash(table, additional, hasher);
}
