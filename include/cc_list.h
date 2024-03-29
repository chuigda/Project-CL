#ifndef PROJECT_CL_LIST_H
#define PROJECT_CL_LIST_H

#include <cc_defs.h>

#ifndef PROJECT_CL_BUILD_LIST
#   error PROJECT_CL_BUILD_LIST not defined
#endif

#include <cc_error.h>

#ifdef __cplusplus
extern "C" {
#endif

struct st_cc_list;

typedef struct st_cc_list cc_list;
typedef struct st_cc_list_node cc_list_iter;

cc_list *cc_list_init(cc_size item_size, cc_dtor dtor);

void cc_list_destroy(cc_list *list);

void cc_list_dtor(void *list);

cc_size cc_list_size(cc_list *list);

_Bool cc_list_empty(cc_list *list);

void *cc_list_front(cc_list *list);

void *cc_list_back(cc_list *list);

cc_list_iter *cc_list_begin(cc_list *list);

cc_list_iter *cc_list_end(cc_list *list);

cc_list_iter *cc_list_iter_prev(cc_list_iter *iter);

cc_list_iter *cc_list_iter_next(cc_list_iter *iter);

void *cc_list_iter_deref(cc_list_iter *iter);

cc_error cc_list_push(cc_list *list, void *value);

cc_error cc_list_push_front(cc_list *list, void *value);

cc_error cc_list_insert(cc_list *list, cc_list_iter *iter, void *value);

cc_error cc_list_remove(cc_list *list, cc_list_iter *iter);

cc_error cc_list_remove_if(cc_list *list, cc_pred pred);

cc_error cc_list_remove_if2(cc_list *list, cc_pred2 pred, void *ctx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_LIST_H */
