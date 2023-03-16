#ifndef PROJECT_CL_VEC_H
#define PROJECT_CL_VEC_H

#include <cc_defs.h>

#ifndef PROJECT_CL_BUILD_VEC
#   error PROJECT_CL_BUILD_VEC not defined
#endif

#include <cc_error.h>

#ifdef __cplusplus
extern "C" {
#endif

struct st_cc_vec;

typedef struct st_cc_vec cc_vec;

cc_vec *cc_vec_init(cc_size item_size);

cc_vec *cc_vec_init2(cc_size item_size, cc_size init_cap);

void cc_vec_destroy(cc_vec *vec);

void cc_vec_dtor(void *vec);

cc_size cc_vec_size(cc_vec *vec);

cc_size cc_vec_cap(cc_vec *vec);

cc_error cc_vec_shrink(cc_vec *vec);

cc_error cc_vec_reserve(cc_vec *vec, cc_size cap);

void *cc_vec_at(cc_vec *vec, cc_size idx);

cc_error cc_vec_push(cc_vec *vec, void *value);

cc_error cc_vec_insert(cc_vec *vec, cc_size idx, void *value);

cc_error
cc_vec_insert2(cc_vec *vec, cc_size idx, void *value, cc_size cnt);

cc_error cc_vec_pop(cc_vec *vec);

cc_error cc_vec_pop_front(cc_vec *vec);

cc_error cc_vec_remove(cc_vec *vec, cc_size idx);

cc_error cc_vec_remove2(cc_vec *vec, cc_size idx, cc_size cnt);

cc_error cc_vec_remove_if(cc_vec *vec, cc_pred pred, cc_dtor dtor);

cc_error
cc_vec_remove_if2(cc_vec *vec, cc_pred2 pred, void *ctx, cc_dtor dtor);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_VEC_H */
