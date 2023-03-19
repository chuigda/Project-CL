#include <cc_cfg.h>

#ifdef PROJECT_CL_BUILD_VEC

#include <cc_alloc.h>
#include <cc_assert.h>
#include <cc_vec.h>
#include <cc_memory.h>

#include <cc_impl.h>

struct st_cc_vec {
    void *buf;
    cc_dtor *dtor;
    cc_size size;
    cc_size cap;
    cc_size item_size;
};

cc_vec *cc_vec_init(cc_size item_size, cc_dtor dtor) {
    RT_CONTRACT2(item_size > 0, NULL)

    cc_vec *ret = cc_alloc(sizeof(cc_vec));
    if (ret) {
        cc_memset(ret, 0, sizeof(cc_vec));
        ret->dtor = dtor;
        ret->item_size = item_size;
    }
    return ret;
}

cc_vec*
cc_vec_init2(cc_size item_size, cc_dtor dtor, cc_size init_cap) {
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

        ret->dtor = dtor;
        ret->item_size = item_size;
    }
    return ret;
}

void cc_vec_destroy(cc_vec *vec) {
    if (!vec) {
        return;
    }

    if (vec->dtor) {
        for (cc_size i = 0; i < vec->size; i++) {
            void *item = CC_PTR_OFFSET2(vec->buf, vec->item_size, i);
            vec->dtor(item);
        }
    }

    cc_free(vec->buf);
    cc_free(vec);
}

void cc_vec_dtor(void *vec) {
    cc_vec_destroy((cc_vec*)vec);
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

void *cc_vec_front(cc_vec *vec) {
    RT_CONTRACT2(vec, NULL);
    if (vec->size == 0) {
        return NULL;
    }

    return vec->buf;
}

void *cc_vec_back(cc_vec *vec) {
    RT_CONTRACT2(vec, NULL);
    if (vec->size == 0) {
        return NULL;
    }

    return CC_PTR_OFFSET2(vec->buf, vec->item_size, vec->size - 1);
}

cc_error cc_vec_push(cc_vec *vec, void *value) {
    RT_CONTRACT_E(vec && value)

    if (vec->size == vec->cap) {
        cc_size newcap;
        if (!vec->buf) {
            newcap = 4;
        } else {
            newcap = vec->cap * 2;
        }

        void *newbuf = cc_alloc(vec->item_size * newcap);
        if (!newbuf) {
            return CC_OUT_OF_MEMORY;
        }
        if (vec->buf) {
            cc_memcpy(newbuf, vec->buf, vec->size * vec->item_size);
        }
        cc_memcpy(CC_PTR_OFFSET2(newbuf, vec->item_size, vec->size),
                  value,
                  vec->item_size);
        cc_free(vec->buf);
        vec->buf = newbuf;
        vec->cap = newcap;
    } else {
        cc_memcpy(CC_PTR_OFFSET2(vec->buf, vec->item_size, vec->size),
                  value,
                  vec->item_size);
    }
    vec->size += 1;
    return CC_NO_ERROR;
}

cc_error cc_vec_insert(cc_vec *vec, cc_size idx, void *value) {
    return cc_vec_insert2(vec, idx, value, 1);
}

cc_error
cc_vec_insert2(cc_vec *vec, cc_size idx, void *value, cc_size cnt) {
    RT_CONTRACT_E(vec && value && idx <= vec->size)
    if (cnt == 0) {
        return CC_NO_ERROR;
    }

    if (!vec->buf) {
        vec->buf = cc_alloc(vec->item_size * cnt);
        if (!vec->buf) {
            return CC_OUT_OF_MEMORY;
        }
        vec->size = cnt;
        vec->cap = cnt;
        cc_memcpy(vec->buf, value, vec->item_size * cnt);
    } else if (vec->size + cnt < vec->cap) {
        void *dst = CC_PTR_OFFSET2(vec->buf, vec->item_size, idx + cnt);
        void *src = CC_PTR_OFFSET2(vec->buf, vec->item_size, idx);
        cc_size sz = (vec->size - idx) * vec->item_size;
        cc_memmove(dst, src, sz);
        cc_memmove(src, value, vec->item_size * cnt);
    } else {
        cc_size newcap = (vec->cap * 2 > vec->size + cnt) 
            ? vec->cap * 2
            : vec->size + cnt;
        void *newbuf = cc_alloc(vec->item_size * newcap);
        if (!newbuf) {
            return CC_OUT_OF_MEMORY;
        }
        cc_memcpy(newbuf, vec->buf, vec->item_size * idx);
        cc_memcpy(CC_PTR_OFFSET2(newbuf, vec->item_size, idx),
                  value,
                  vec->item_size * cnt);
        cc_memcpy(CC_PTR_OFFSET2(newbuf, vec->item_size, idx + cnt),
                  CC_PTR_OFFSET2(vec->buf, vec->item_size, idx),
                  vec->item_size * (vec->size - idx));
        cc_free(vec->buf);
        vec->buf = newbuf;
        vec->cap = newcap;
        vec->size += cnt;
    }

    return CC_NO_ERROR;
}

cc_error cc_vec_pop(cc_vec *vec) {
    RT_CONTRACT_E(vec)
    if (vec->size != 0) {
        vec->size -= 1;
        if (vec->dtor) {
            void *item = CC_PTR_OFFSET2(vec->buf,
                                        vec->item_size,
                                        vec->size);
            vec->dtor(item);
        }
    }
    return CC_NO_ERROR;
}

cc_error cc_vec_pop_front(cc_vec *vec) {
    return cc_vec_remove(vec, 0);
}

cc_error cc_vec_remove(cc_vec *vec, cc_size idx) {
    return cc_vec_remove2(vec, idx, 1);
}

cc_error cc_vec_remove2(cc_vec *vec, cc_size idx, cc_size cnt) {
    RT_CONTRACT_E(vec && idx < vec->size && idx + cnt <= vec->size)
    if (cnt == 0) {
        return CC_NO_ERROR;
    }
    if (idx + cnt == vec->size) {
        vec->size -= cnt;
        return CC_NO_ERROR;
    }

    if (vec->dtor) {
        for (cc_size i = idx; i < idx + cnt; i++) {
            void *item = CC_PTR_OFFSET2(vec->buf, vec->item_size, i);
            vec->dtor(item);
        }
    }

    cc_memmove(CC_PTR_OFFSET2(vec->buf, vec->item_size, idx),
               CC_PTR_OFFSET2(vec->buf, vec->item_size, idx + cnt),
               (vec->size - idx - cnt) * vec->item_size);
    vec->size -= cnt;
    return CC_NO_ERROR;
}

static inline _Bool cc_vec_internal_remove_if_pred2(void *ptr, void *ctx) {
    cc_pred ** fn = (cc_pred**) ctx;
    return (*fn)(ptr);
}

cc_error cc_vec_remove_if(cc_vec *vec, cc_pred pred) {
    return cc_vec_remove_if2(vec, cc_vec_internal_remove_if_pred2, (void *)(&pred));
}

cc_error
cc_vec_remove_if2(cc_vec *vec, cc_pred2 pred, void *ctx) {
    RT_CONTRACT_E(vec && pred);

    cc_size removed = 0;
    for (cc_size i = 0; i < vec->size; i++) {
        void *value = CC_PTR_OFFSET2(vec->buf, vec->item_size, i);
        if (pred(value, ctx)) {
            if (vec->dtor) {
                vec->dtor(value);
            }
            removed += 1;
        } else {
            if (removed != 0) {
                void *dst = CC_PTR_OFFSET2(vec->buf,
                                           vec->item_size,
                                           i - removed);
                cc_memcpy(dst, value, vec->item_size);
            }
        }
    }
    vec->size -= removed;

    return CC_NO_ERROR;
}

#endif /* PROJECT_CL_BUILD_VEC */
