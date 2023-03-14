#include <cc_cfg.h>

#ifdef PROJECT_CL_BUILD_VEC

#include <cc_alloc.h>
#include <cc_assert.h>
#include <cc_vec.h>
#include <cc_memory.h>

#include <cc_impl.h>

struct st_cc_vec {
    void *buf;
    cc_size end;
    cc_size cap;
    cc_size item_size;
};

cc_vec *cc_vec_init(cc_size item_size) {
    RT_CONTRACT2(item_size > 0, NULL);

    cc_vec *ret = cc_alloc(sizeof(cc_vec));
    if (ret) {
        cc_memset(ret, 0, sizeof(cc_vec));
        ret->item_size = item_size;
    }
    return ret;
}

cc_vec *cc_vec_init2(cc_size item_size, cc_size init_cap) {
    RT_CONTRACT2(item_size > 0 && init_cap > 0, NULL);

    cc_vec *ret = cc_alloc(sizeof(cc_vec));
    if (ret) {
        ret->buf = cc_alloc(item_size * init_cap);
        if (ret->buf) {
            ret->end = 0;
            ret->cap = init_cap;
        } else {
            cc_memset(ret, 0, sizeof(cc_vec));
        }
        ret->item_size = item_size;
    }
    return ret;
}

void cc_vec_destroy(cc_vec *vec) {
    if (!vec) {
        return;
    }

    cc_free(vec->buf);
    cc_free(vec);
}

cc_size cc_vec_size(cc_vec *vec) {
    RT_CONTRACT2(vec, 0);

    return vec->end;
}

cc_size cc_vec_cap(cc_vec *vec) {
    RT_CONTRACT2(vec, 0);

    return vec->cap;
}

void cc_vec_shrink(cc_vec *vec) {
    RT_CONTRACT(vec);
}

#endif /* PROJECT_CL_BUILD_VEC */
