#ifndef PROJECT_CL_MAP_H
#define PROJECT_CL_MAP_H

#include <cc_defs.h>

#ifndef PROJECT_CL_BUILD_MAP
#   error PROJECT_CL_BUILD_MAP not defined
#endif

#include <cc_error.h>

#ifdef __cplusplus
extern "C" {
#endif

struct st_cc_map;

typedef struct st_cc_map cc_map;

typedef cc_size (cc_hash)(void *data);

cc_map*
cc_map_init(cc_size key_size,
            cc_size value_size,
            cc_hash hash,
            cc_dtor dtor);

cc_map*
cc_map_init2(cc_size key_size,
             cc_size value_size,
             cc_hash hash,
             cc_dtor dtor,
             cc_size init_bucket_cnt);

void cc_map_destroy(cc_map *map);

void cc_map_dtor(void *map);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_MAP_H */
