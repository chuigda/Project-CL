#include <cc_cfg.h>

#ifdef PROJECT_CL_BUILD_LIST

#include <cc_list.h>

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

    cc_size item_size;
};

#endif /* PROJECT_CL_BUILD_LIST */
