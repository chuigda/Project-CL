#include <cc_alloc.h>
#include <cc_hashmap.h>
#include <cc_memory.h>
#include <cc_swisstable/cc_swisstable.h>

#ifndef CC_HASHMAP_STACK_ALLOC_THRESHOLD
#   define CC_HASHMAP_STACK_ALLOC_THRESHOLD 64
#endif

struct st_cc_hashmap
{
    cc_dtor * key_dtor;
    cc_dtor * value_dtor;
    cc_data_layout key_layout;
    cc_data_layout value_layout;
    void * randomness;
    cc_hash * hash;
    cc_pred2 * equal;
    cc_swisstable table;
};

#define cc_hashmap_load_layout(MAP)               \
   cc_data_layout key_layout = (MAP)->key_layout; \
   cc_data_layout value_layout = (MAP)->value_layout;

#define cc_hashmap_alignment()                    \
   (key_layout.alignment > value_layout.alignment \
        ? key_layout.alignment                    \
        : value_layout.alignment)

#define cc_hashmap_value_offset() \
   (key_layout.size               \
    + ((-key_layout.size) & (value_layout.alignment - 1)))

#define cc_hashmap_element_size() \
   (cc_hashmap_value_offset() + value_layout.size)

#define cc_hashmap_get_value(key) \
   ((void *)(&((cc_uint8 *)(key))[cc_hashmap_value_offset()]))

cc_hashmap * cc_hashmap_create(cc_data_layout key_layout,
                               cc_data_layout value_layout,
                               cc_dtor * key_dtor,
                               cc_dtor * value_dtor,
                               void * randomness,
                               cc_hash * hash,
                               cc_pred2 * equal)
{
    cc_size alignment = cc_hashmap_alignment();
    cc_size size = cc_hashmap_element_size();
    cc_hashmap * hashmap = (cc_hashmap *)cc_alloc(sizeof(cc_hashmap));
    if (!hashmap)
    {
        return NULL;
    }
    hashmap->key_dtor = key_dtor;
    hashmap->value_dtor = value_dtor;
    hashmap->key_layout = key_layout;
    hashmap->value_layout = value_layout;
    hashmap->randomness = randomness;
    hashmap->hash = hash;
    hashmap->table = cc_swisstable_empty(size, alignment);
    hashmap->equal = equal;
    return hashmap;
}

cc_hashmap * cc_hashmap_with_capacity(cc_data_layout key_layout,
                                      cc_data_layout value_layout,
                                      cc_dtor * key_dtor,
                                      cc_dtor * value_dtor,
                                      void * randomness,
                                      cc_hash * hash,
                                      cc_pred2 * equal,
                                      cc_size capacity)
{
    cc_size alignment = cc_hashmap_alignment();
    cc_size size = cc_hashmap_element_size();
    cc_hashmap * hashmap = (cc_hashmap *)cc_alloc(sizeof(cc_hashmap));
    if (!hashmap)
    {
        return NULL;
    }
    hashmap->table
        = cc_swisstable_with_capacity(size, alignment, capacity);
    if (!hashmap->table.ctrl)
    {
        cc_free(hashmap);
        return NULL;
    }
    hashmap->key_dtor = key_dtor;
    hashmap->value_dtor = value_dtor;
    hashmap->key_layout = key_layout;
    hashmap->value_layout = value_layout;
    hashmap->randomness = randomness;
    hashmap->hash = hash;
    hashmap->equal = equal;
    return hashmap;
}

void cc_hashmap_destroy(cc_hashmap * map)
{
    cc_hashmap_load_layout(map);
    cc_swisstable_iter iter = cc_swisstable_create_iter(&map->table);
    void * key = cc_swisstable_iter_next(&iter);
    while (key != NULL)
    {
        if (map->key_dtor)
        {
            map->key_dtor(key);
        }
        if (map->value_dtor)
        {
            map->value_dtor(cc_hashmap_get_value(key));
        }
        key = cc_swisstable_iter_next(&iter);
    }
    cc_swisstable_destroy(&map->table);
    cc_free(map);
}

static inline void *
cc_hashmap_insert_unchecked(cc_hashmap * map, void * key, void * value)
{
    cc_hashmap_load_layout(map);
    cc_swisstable_hasher hasher = (cc_swisstable_hasher){
        .hash = map->hash,
        .state = map->randomness,
    };
    void * kv_buffer = NULL;
    _Bool kv_buffer_is_on_heap = 0;
#if defined(__has_builtin) && __has_builtin(__builtin_alloca)
    if (cc_hashmap_element_size() <= CC_HASHMAP_STACK_ALLOC_THRESHOLD)
    {
        kv_buffer = __builtin_alloca(cc_hashmap_element_size());
    }
#endif
    if (kv_buffer == NULL)
    {
        kv_buffer = cc_alloc(cc_hashmap_element_size());
        kv_buffer_is_on_heap = 1;
    }
    if (kv_buffer == NULL)
    {
        return NULL;
    }
    cc_memcpy(kv_buffer, key, key_layout.size);
    cc_memcpy(cc_hashmap_get_value(kv_buffer), value, value_layout.size);
    void * ptr = cc_swisstable_insert(&map->table, kv_buffer, hasher);
    if (kv_buffer_is_on_heap)
    {
        cc_free(kv_buffer);
    }
    return ptr;
}

void * cc_hashmap_key2value(const cc_hashmap * map, void * key)
{
    cc_hashmap_load_layout(map);
    return cc_hashmap_get_value(key);
}

static inline cc_swisstable_hasher
cc_hashmap_hasher_(const cc_hashmap * map)
{
    return (cc_swisstable_hasher){.hash = map->hash,
                                  .state = map->randomness};
}

void * cc_hashmap_find(const cc_hashmap * map, void * key)
{
    return cc_swisstable_find(&map->table,
                              key,
                              cc_hashmap_hasher_(map),
                              map->equal);
}

void cc_hashmap_erase(cc_hashmap * map, void * key)
{
    void * slot = cc_swisstable_find(&map->table,
                                     key,
                                     cc_hashmap_hasher_(map),
                                     map->equal);
    if (slot)
    {
        cc_swisstable_erase(&map->table, slot);
    }
}

void * cc_hashmap_insert(cc_hashmap * map, void * key, void * value)
{
    cc_hashmap_load_layout(map);
    void * slot = cc_swisstable_find(&map->table,
                                     key,
                                     cc_hashmap_hasher_(map),
                                     map->equal);
    if (slot)
    {
        cc_memcpy(cc_hashmap_get_value(slot), value, value_layout.size);
        return slot;
    }
    else
    {
        return cc_hashmap_insert_unchecked(map, key, value);
    }
}

cc_size cc_hashmap_size(const cc_hashmap * map)
{
    return map->table.items;
}

cc_size cc_hashmap_growth_left(const cc_hashmap * map)
{
    return map->table.growth_left;
}

_Bool cc_hashmap_reserve(cc_hashmap * map, cc_size additional)
{
    return cc_swisstable_reserve(&map->table,
                                 additional,
                                 cc_hashmap_hasher_(map));
}

void cc_hashmap_visit(cc_hashmap * map,
                      cc_hashmap_visitor * visitor,
                      void * context)
{
    cc_hashmap_load_layout(map);
    cc_swisstable_iter iter = cc_swisstable_create_iter(&map->table);
    void * slot = cc_swisstable_iter_next(&iter);
    while (slot)
    {
        visitor(slot, cc_hashmap_get_value(slot), context);
        slot = cc_swisstable_iter_next(&iter);
    }
}
