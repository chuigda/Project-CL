//
// Created by Yifan Zhu on 3/26/23.
//

#ifndef PROJECT_CL_SWISSTABLE_H
#define PROJECT_CL_SWISSTABLE_H

#include <cc_defs.h>

typedef struct {
    void *state;

    cc_uint64 (*hash)(void *state, const void *key, cc_size key_size);
} cc_swisstable_hasher;

// for internal usage thus expose details
typedef struct {
    cc_size bucket_mask;
    cc_size element_size;
    cc_size element_alignment;
    void *ctrl;
    cc_size growth_left;
    cc_size items;
} cc_swisstable;

// for internal usage thus expose details
typedef struct {
    void *bucket_ptr;
    cc_uint8 *next;
    cc_size element_size;
    cc_size elems;
    cc_uint64 opaque;
} cc_swisstable_iter;

void *cc_swisstable_insert(cc_swisstable *table, const void *element,
                           cc_swisstable_hasher hasher);

cc_swisstable cc_swisstable_empty(cc_size element_size,
                                  cc_size element_alignment);

cc_swisstable cc_swisstable_with_capacity(cc_size element_size,
                                          cc_size element_alignment,
                                          cc_size capacity);

void *cc_swisstable_find(cc_swisstable *table, void *element,
                         cc_swisstable_hasher hasher, cc_pred2 equal);

void cc_swisstable_erase(cc_swisstable *table, void *element);

void cc_swisstable_destroy(cc_swisstable *table);

#endif // PROJECT_CL_SWISSTABLE_H
