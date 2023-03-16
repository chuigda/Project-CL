#include <cc_cfg.h>

#ifdef PROJECT_CL_BUILD_LIST

#include <cc_alloc.h>
#include <cc_assert.h>
#include <cc_list.h>
#include <cc_memory.h>
#include <cc_impl.h>

struct st_cc_list_node {
#   ifdef PROJECT_CL_RUNTIME_CHECK
    _Bool is_tail;
#   endif
    struct st_cc_list_node *prev;
    struct st_cc_list_node *next;

    cc_uint8 data[];
};

typedef struct st_cc_list_node cc_list_node;

struct st_cc_list {
#   ifdef PROJECT_CL_RUNTIME_CHECK
    _Bool is_tail;
#   endif

    cc_list_node *prev;
    cc_list_node *next;

    cc_dtor *dtor;
    cc_size item_size;
};

cc_list *cc_list_init(cc_size item_size, cc_dtor dtor) {
    RT_CONTRACT2(item_size, NULL)

    cc_list *ret = cc_alloc(sizeof(cc_list));
    if (ret) {
        RT_CHECK({ ret->is_tail = 1; })
        ret->prev = (cc_list_node*)ret;
        ret->next = (cc_list_node*)ret;
        ret->dtor = dtor;
        ret->item_size = item_size;
    }

    return ret;
}

void cc_list_destroy(cc_list *list) {
    if (!list) {
        return;
    }

    cc_list_iter *it = list->next;
    while (it != (cc_list_iter*)list) {
        if (list->dtor) {
            list->dtor(it->data);
        }

        cc_list_iter *t = it;
        it = it->next;
        cc_free(t);
    }
    cc_free(list);
}

void cc_list_dtor(void *list) {
    cc_list_destroy((cc_list*)list);
}

cc_size cc_list_size(cc_list *list) {
    RT_CONTRACT2(list, 0)

    cc_size ret = 0;
    for (cc_list_iter *it = list->next;
         it != (cc_list_iter*)list;
         it = it->next)
    {
        ret += 1;
    }
    return ret;
}

_Bool cc_list_empty(cc_list *list) {
    RT_CONTRACT2(list, 0)

    return list->next == (cc_list_node*)list;
}

void *cc_list_front(cc_list *list) {
    RT_CONTRACT2(list, NULL)
    if (list->next == (cc_list_iter*)list) {
        return NULL;
    }

    return list->next->data;
}

void *cc_list_back(cc_list *list) {
    RT_CONTRACT2(list, NULL)
    if (list->prev == (cc_list_iter*)list) {
        return NULL;
    }

    return list->prev->data;
}

cc_list_iter *cc_list_begin(cc_list *list) {
    RT_CONTRACT2(list, NULL)
    return list->next;
}

cc_list_iter *cc_list_end(cc_list *list) {
    RT_CONTRACT2(list, NULL)
    return (cc_list_iter*)list;
}

cc_list_iter *cc_list_iter_prev(cc_list_iter *iter) {
    RT_CONTRACT2(iter, NULL)
    return iter->prev;
}

cc_list_iter *cc_list_iter_next(cc_list_iter *iter) {
    RT_CONTRACT2(iter, NULL)
    return iter->next;
}

void *cc_list_iter_deref(cc_list_iter *iter) {
    RT_CONTRACT2(iter && !iter->is_tail, NULL)
    return iter->data;
}

static cc_list_node *
cc_list_make_node(cc_size item_size, void *item) {
    cc_list_node *node = cc_alloc(sizeof(cc_list_node) + item_size);
    if (node) {
        RT_CHECK({ node->is_tail = 0; })
        cc_memcpy(node->data, item, item_size);
    }
    return node;
}

cc_error cc_list_push(cc_list *list, void *value) {
    RT_CONTRACT_E(list && value)

    cc_list_node *node = cc_list_make_node(list->item_size, value);
    if (!node) {
        return CC_OUT_OF_MEMORY;
    }

    node->prev = list->prev;
    node->next = (cc_list_node*)list;
    list->prev->next = node;
    list->prev = node;
    return CC_NO_ERROR;
}

cc_error cc_list_push_front(cc_list *list, void *value) {
    RT_CONTRACT_E(list && value)

    cc_list_node *node = cc_list_make_node(list->item_size, value);
    if (!node) {
        return CC_OUT_OF_MEMORY;
    }

    node->prev = (cc_list_node*)list;
    node->next = list->next;
    list->next->prev = node;
    list->next = node;
    return CC_NO_ERROR;
}

cc_error
cc_list_insert(cc_list *list, cc_list_iter *iter, void *value) {
    RT_CONTRACT_E(list && iter && value)

    cc_list_node *node = cc_list_make_node(list->item_size, value);
    if (!node) {
        return CC_OUT_OF_MEMORY;
    }

    node->prev = iter->prev;
    node->next = iter;
    iter->prev->next = node;
    iter->prev = node;
    return CC_NO_ERROR;
}

cc_error
cc_list_remove(cc_list *list, cc_list_iter *iter) {
    RT_CONTRACT_E(list && iter && !iter->is_tail);
    
    if (list->dtor) {
        list->dtor(iter->data);
    }
    iter->prev->next = iter->next;
    iter->next->prev = iter->prev;
    cc_free(iter);

    return CC_NO_ERROR;
}

#endif /* PROJECT_CL_BUILD_LIST */
