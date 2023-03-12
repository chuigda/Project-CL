#ifdef PROJECT_CL_BUILD_VEC

#include <cc_alloc.h>
#include <cc_assert.h>
#include <cc_vec.h>
#include <cc_memory.h>

struct st_cc_vec {
    void *begin;
    void *end;
    void *cap;

    cc_size elem_size;
};

cc_vec *cc_vec_init(cc_size item_size) {
#ifdef PROJECT_CL_RUNTIME_CHECK
    cc_assert(item_size > 0);
    if (item_size == 0) {
        return NULL;
    }
#endif

    cc_vec *ret = cc_alloc(sizeof(cc_vec));
    if (ret) {
        cc_memset(ret, 0, sizeof(cc_vec));
    }
    return ret;
}

#endif /* PROJECT_CL_BUILD_VEC */
