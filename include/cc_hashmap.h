#ifndef PROJECT_CL_MAP_H
#define PROJECT_CL_MAP_H

#include <cc_defs.h>

#ifndef PROJECT_CL_BUILD_HASHMAP
#   error PROJECT_CL_BUILD_HASHMAP not defined
#endif

#include <cc_error.h>

#ifdef __cplusplus
extern "C" {
#endif

struct st_cc_hashmap;

typedef struct st_cc_hashmap cc_hashmap;

typedef cc_uint64(cc_hash)(void * randomness, const void * data);

typedef void(cc_hashmap_visitor)(void * key,
                                 void * value,
                                 void * context);

cc_hashmap * cc_hashmap_create(cc_data_layout key_layout,
                               cc_data_layout value_layout,
                               cc_dtor * key_dtor,
                               cc_dtor * value_dtor,
                               void * randomness,
                               cc_hash * hash,
                               cc_pred2 * equal);

cc_hashmap * cc_hashmap_with_capacity(cc_data_layout key_layout,
                                      cc_data_layout value_layout,
                                      cc_dtor * key_dtor,
                                      cc_dtor * value_dtor,
                                      void * randomness,
                                      cc_hash * hash,
                                      cc_pred2 * equal,
                                      cc_size capacity);

void cc_hashmap_destroy(cc_hashmap * map);

void * cc_hashmap_insert(cc_hashmap * map, void * key, void * value);

void * cc_hashmap_find(const cc_hashmap * map, void * key);

void cc_hashmap_erase(cc_hashmap * map, void * key);

cc_size cc_hashmap_size(const cc_hashmap * map);

cc_size cc_hashmap_growth_left(const cc_hashmap * map);

_Bool cc_hashmap_reserve(cc_hashmap * map, cc_size additional);

void cc_hashmap_visit(cc_hashmap * map,
                      cc_hashmap_visitor * visitor,
                      void * context);

void * cc_hashmap_key2value(const cc_hashmap * map, void * key);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_MAP_H */
