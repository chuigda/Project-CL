#include <cc_cfg.h>

#ifdef PROJECT_CL_BUILD_VEC

#include <cc_alloc.h>
#include <cc_assert.h>
#include <cc_vec.h>
#include <cc_memory.h>

#include <cc_impl.h>

struct st_cc_vec {
    void *buf;
    cc_size size;
    cc_size cap;
    cc_size item_size;
};

cc_vec *cc_vec_init(cc_size item_size) {
    RT_CONTRACT2(item_size > 0, NULL)

    cc_vec *ret = cc_alloc(sizeof(cc_vec));
    if (ret) {
        cc_memset(ret, 0, sizeof(cc_vec));
        ret->item_size = item_size;
    }
    return ret;
}

cc_vec *cc_vec_init2(cc_size item_size, cc_size init_cap) {
    RT_CONTRACT2(item_size > 0 && init_cap > 0, NULL)

    cc_vec *ret = cc_alloc(sizeof(cc_vec));
    if (ret) {
        ret->buf = cc_alloc(item_size * init_cap);
        if (ret->buf) {
            ret->size = 0;
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
    RT_CONTRACT2(vec, 0)

    return vec->size;
}

cc_size cc_vec_cap(cc_vec *vec) {
    RT_CONTRACT2(vec, 0)

    return vec->cap;
}

cc_error cc_vec_shrink(cc_vec *vec) {
    RT_CONTRACT_E(vec)

    if (vec->size == vec->cap) {
        return CC_NO_ERROR;
    }

    void *newbuf = cc_alloc(vec->size * vec->item_size);
    if (!newbuf) {
        return CC_OUT_OF_MEMORY;
    }

    cc_memcpy(newbuf, vec->buf, vec->size * vec->item_size);
    cc_free(vec->buf);
    vec->buf = newbuf;
    vec->cap = vec->size;

    return CC_NO_ERROR;
}

cc_error cc_vec_reserve(cc_vec *vec, cc_size cap) {
    RT_CONTRACT_E(vec && cap > 0)
    
    if (vec->cap >= cap) {
        return CC_NO_ERROR;
    }

    void *newbuf = cc_alloc(cap * vec->item_size);
    if (!newbuf) {
        return CC_OUT_OF_MEMORY;
    }

    cc_memcpy(newbuf, vec->buf, vec->size * vec->item_size);
    cc_free(vec->buf);
    vec->buf = newbuf;
    vec->cap = cap;

    return CC_NO_ERROR;
}

void *cc_vec_at(cc_vec *vec, cc_size idx) {
    RT_CONTRACT2(vec, NULL)
    RT_CHECK({
        if (idx >= vec->size) {
            return NULL;
        }
    })

    return CC_PTR_OFFSET2(vec->buf, vec->item_size, idx);
}

#endif /* PROJECT_CL_BUILD_VEC */
