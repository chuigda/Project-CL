#include "cc_list.h"

#include "cc_alloc.h"
#include "cc_assert.h"
#include "cc_inttypes.h"
#include "cc_string.h"

#if CCLIB_DS_LLIST != CC_FEAT_DISABLED

#define CC_UINT8 CCTY(cc_uint8)
#define CC_SIZE CCTY(cc_size)
#define CC_LIST CCTY(cc_list)

#define CC_LIST_ITER CCTY(cc_list_iter)

#define CC_ALLOC CCFN(cc_alloc)
#define CC_FREE CCFN(cc_free)
#define CC_MEMCPY CCFN(cc_memcpy)

typedef struct st_list_node {
    struct st_list_node *prev;
    struct st_list_node *next;
    CC_UINT8 data[0];
} list_node;

void
CCFN(cc_list_init) (CC_LIST *list, CC_SIZE elem_size) {
    list_node *head_node = (list_node*)list;
    head_node->prev = head_node;
    head_node->next = head_node;
    
    list->elem_size = elem_size;
}

void
CCFN(cc_list_destroy) (CC_LIST *list) {
    list_node *head_node = (list_node*)list;
    list_node *iter_node = head_node->next;
    while (iter_node != head_node) {
        list_node *t = iter_node;
        iter_node = iter_node->next;
        CC_FREE (t);
    }
}

void
CCFN(cc_list_push_back) (CC_LIST *list, const void *data) {
    list_node *new_node = 
        (list_node*)CC_ALLOC(sizeof(list_node) + list->elem_size);
    CC_MEMCPY (CC_VPTR_ADD(new_node, CC_OFFSETOF(list_node, data)),
               data,
               list->elem_size);
    
    list_node *head_node = (list_node*)list;
    new_node->prev = head_node->prev;
    new_node->next = head_node;
    head_node->prev->next = new_node;
    head_node->prev = new_node;
}

void
CCFN(cc_list_push_front) (CCTY(cc_list) *list, const void *data) {
    list_node *new_node = 
        (list_node*)CC_ALLOC(sizeof(list_node) + list->elem_size);
    CC_MEMCPY (CC_VPTR_ADD(new_node, CC_OFFSETOF(list_node, data)),
               data,
               list->elem_size);
    
    list_node *head_node = (list_node*)list;
    new_node->prev = head_node;
    new_node->next = head_node->next;
    head_node->next->prev = new_node;
    head_node->next = new_node;
}

void
CCFN(cc_list_pop_back) (CCTY(cc_list) *list) {
    CC_ASSERT(!CCFN(cc_list_empty)(list));
    
    list_node *head_node = (list_node*)list;
    list_node *back_node = head_node->prev;
    back_node->prev->next = back_node->next;
    back_node->next->prev = back_node->prev;
    CC_FREE(back_node);
}

void
CCFN(cc_list_pop_front) (CCTY(cc_list) *list) {
    CC_ASSERT(!CCFN(cc_list_empty)(list));
    
    list_node *head_node = (list_node*)list;
    list_node *front_node = head_node->next;
    front_node->next->prev = front_node->prev;
    front_node->prev->next = front_node->next;
    CC_FREE(front_node);
}

void*
CCFN(cc_list_front) (const CCTY(cc_list) *list) {
    CC_ASSERT(!CCFN(cc_list_empty)(list));

    list_node *head_node = (list_node*)list;
    list_node *front_node = head_node->next;
    return CC_VPTR_ADD(front_node, CC_OFFSETOF(list_node, data));
}

void*
CCFN(cc_list_back) (const CCTY(cc_list) *list) {
    CC_ASSERT(!CCFN(cc_list_empty)(list));
    
    list_node *head_node = (list_node*)list;
    list_node *back_node = head_node->prev;
    return CC_VPTR_ADD(back_node, CC_OFFSETOF(list_node, data));
}

CCTY(cc_size)
CCFN(cc_list_len) (const CCTY(cc_list) *list) {
    return CCFN(cc_list_size) (list);
}

CCTY(cc_size)
CCFN(cc_list_size) (const CCTY(cc_list) *list) {
    CCTY(cc_size) ret = 0;
    list_node *head_node = (list_node*)list;
    list_node *iter_node = head_node->next;
    while (iter_node != head_node) {
        iter_node = iter_node->next;
        ret++;
    }
    return ret;
}

_Bool
CCFN(cc_list_empty) (const CCTY(cc_list) *list) {
    list_node *head_node = (list_node*)list;
    return head_node->next == head_node;
}

CCTY(cc_list_iter)
CCFN(cc_list_begin) (const CCTY(cc_list) *list) {
    CCTY(cc_list_iter) ret;
    list_node *head_node = (list_node*)list;
    ret.node = head_node->next;
    return ret;
}

CCTY(cc_list_iter)
CCFN(cc_list_end) (const CCTY(cc_list) *list) {
    CCTY(cc_list_iter) ret;
    ret.node = (CCTY(cc_list)*)list;
    return ret;
}

void
CCFN(cc_list_insert) (const CCTY(cc_list) *list,
                      CCTY(cc_list_iter) pos,
                      const void *data) {
    list_node *new_node = 
        (list_node*)CC_ALLOC(sizeof(list_node) + list->elem_size);
    CC_MEMCPY (CC_VPTR_ADD(new_node, CC_OFFSETOF(list_node, data)),
               data,
               list->elem_size);
    list_node *node = (list_node*)pos.node;
    new_node->prev = node->prev;
    new_node->next = node;
    node->prev->next = new_node;
    node->prev = new_node;
}

void
CCFN(cc_list_remove) (const CCTY(cc_list) *list,
                      CCTY(cc_list_iter) pos) {
    (void)list;
    list_node *node = (list_node*)pos.node;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    CC_FREE (node);
}

void
CCFN(cc_list_remove_range) (const CCTY(cc_list) *list,
                            CCTY(cc_list_iter) first,
                            CCTY(cc_list_iter) last) {
    (void)list;
    list_node *first_node = (list_node*)first.node;
    list_node *last_node = (list_node*)last.node;
    first_node->prev->next = last_node;
    last_node->prev = first_node->prev;
    
    while (first_node != last_node) {
        list_node *t = first_node;
        first_node = first_node->next;
        CC_FREE (t);
    }
}

void*
CCFN(cc_list_iter_deref) (CCTY(cc_list_iter) iter) {
    list_node *node = (list_node*)iter.node;
    return CC_VPTR_ADD(node, CC_OFFSETOF(list_node, data));
}

CCTY(cc_list_iter)
CCFN(cc_list_iter_prev) (CCTY(cc_list_iter) iter) {
    CCTY(cc_list_iter) ret;
    list_node *node = (list_node*)iter.node;
    ret.node = node->prev;
    return ret;
}

CCTY(cc_list_iter)
CCFN(cc_list_iter_next) (CCTY(cc_list_iter) iter) {
    CCTY(cc_list_iter) ret;
    list_node *node = (list_node*)iter.node;
    ret.node = node->next;
    return ret;
}

#endif
