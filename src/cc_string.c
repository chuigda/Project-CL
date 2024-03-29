#include <cc_cfg.h>

#ifdef PROJECT_CL_BUILD_STRING

#include <string.h>

#include <cc_assert.h>
#include <cc_string.h>
#include <cc_vec.h>
#include <cc_alloc.h>
#include <cc_error.h>
#include <cc_memory.h>

#include <cc_impl.h>

#define CC_STRING_BACK(s) ((s)->buf[(s)->len])

struct st_cc_string {
    cc_char *buf;
    cc_size len;
    cc_size cap;
};

struct st_cc_string_iter {
    cc_uchar buf[8];
    cc_string *s;
    cc_size idx;
};

cc_string *cc_string_init() {
    cc_string* str = cc_alloc(sizeof(cc_string));
    if (!str) {
        return NULL;
    }
    str->cap = 5;
    str->len = 0;
    str->buf = cc_alloc(sizeof(cc_char) * str->cap);
    if (!str->buf) {
        cc_free(str);
        return NULL;
    }
    return str;
}

void cc_string_destroy(cc_string *s) {
    if (!s) {
        return;
    }
    cc_free(s->buf);
    cc_free(s);
}

static inline cc_error cc_string_resize(cc_string* s, cc_size inc) {
    cc_size tgtlen = s->len + inc;
    if (tgtlen < s->cap) {
        return CC_NO_ERROR;
    }
    // else set cap = 2 * target_len
    cc_size tgtcap = tgtlen << 1;
    // we don't deal with overflows
    char* tmpbuf = (char*)cc_realloc(s->buf, tgtcap * sizeof(char));
    if (!tmpbuf) {
        return CC_OUT_OF_MEMORY;
    }
    s->buf = tmpbuf;
    s->cap = tgtcap;
    return CC_NO_ERROR;
}

cc_error cc_string_push_char(cc_string *s, cc_char c) {
    RT_CONTRACT_E(s);
    cc_error err;
    if ((err = cc_string_resize(s, 1)) != CC_NO_ERROR) {
        return err;
    }
    CC_STRING_BACK(s) = c;
    s->len++;
    return CC_NO_ERROR;
}

cc_uint8 cc_string_push_rune(cc_string *s, cc_rune r) {
    RT_CONTRACT2(s, 0);
    if (r <= 0x7F) {
        cc_string_push_char(s, r);
        return 1;
    }
    if (r <= 0x7FF) {
        cc_string_push_char(s, 0xC0 | (r >> 6));
        cc_string_push_char(s, 0x80 | (r & 0x3F));
        return 2;
    }
    if (r <= 0xFFFF) {
        // detect surrogates
        if (r >= 0xD800 && r <= 0xDFFF) return 0;
        if (r == 0xFFFE || r == 0xFFFF) return 0;
        cc_string_push_char(s, 0xE0 | (r >> 12));
        cc_string_push_char(s, 0x80 | ((r >> 6) & 0x3F));
        cc_string_push_char(s, 0x80 | (r & 0x3F));
        return 3;
    }
    if (r <= 0x10FFFF) {
        cc_string_push_char(s, 0xF0 | (r >> 18));
        cc_string_push_char(s, 0x80 | ((r >> 12) & 0x3F));
        cc_string_push_char(s, 0x80 | ((r >> 6) & 0x3F));
        cc_string_push_char(s, 0x80 | (r & 0x3F));
        return 4;
    }
    return 0;
}

cc_error cc_string_appendn(cc_string *s, const cc_char *s2, cc_size n) {
    RT_CONTRACT_E(s && s2);
    cc_error err;
    if ((err = cc_string_resize(s, n)) != CC_NO_ERROR) {
        return err;
    }
    cc_memcpy(s->buf + s->len, (void *)s2, n * sizeof(cc_char));
    s->len += n;
    return CC_NO_ERROR;
}

cc_error cc_string_appends(cc_string *s, const cc_char *s2) {
    RT_CONTRACT_E(s && s2);
    cc_size s2len = strlen(s2);
    return cc_string_appendn(s, s2, s2len);
}

cc_char cc_string_at(cc_string *s, cc_size idx) {
    RT_CONTRACT2(s && s->len > idx, 0);
    return s->buf[idx];
}

cc_size cc_string_len(cc_string *s) {
    RT_CONTRACT2(s, 0);
    return s->len;
}

cc_size cc_string_cap(cc_string *s) {
    RT_CONTRACT2(s, 0);
    return s->cap;
}

cc_char *cc_string_cstr(cc_string *s) {
    RT_CONTRACT2(s, NULL);
    cc_char *buf = cc_alloc((s->len + 1) * sizeof(cc_char));
    cc_memcpy(buf, s->buf, s->len);
    buf[s->len] = 0;
    return buf;
}

cc_char *cc_string_own_cstr(cc_string *s,
                            cc_size *o_len,
                            cc_size *o_cap) 
{
    RT_CONTRACT2(s, NULL);
    cc_char *buf = s->buf;
    if (o_len) *o_len = s->len;
    if (o_cap) *o_cap = s->cap;
    cc_free(s);
    return buf;
}

cc_string_iter *cc_string_iterator(cc_string *s) {
    cc_string_iter *it = cc_alloc(sizeof(cc_string_iter));
    if (!it) {
        return NULL;
    }
    it->idx = 0;
    it->s = s;
    return it;
}

void cc_string_iter_destroy(cc_string_iter *it) {
    if (!it) {
        return;
    }
    cc_free(it);
}

cc_bool cc_string_iter_has_next(cc_string_iter *it) {
    RT_CONTRACT_E(it);
    return it->idx < it->s->len;
}

cc_error cc_string_iter_next(cc_string_iter *it, cc_rune *o_res) {
    RT_CONTRACT_E(it);

    if (!cc_string_iter_has_next(it)) {
        return CC_END_OF_ITERATOR;
    }
    it->buf[0] = it->s->buf[it->idx];
    it->idx++;
    if ((it->buf[0] & 0x80) == 0) {
        *o_res = (cc_rune)it->buf[0];
        return CC_NO_ERROR;
    }
    if (!cc_string_iter_has_next(it)) {
        return CC_BAD_UTF8_BYTE;
    }
    it->buf[1] = it->s->buf[it->idx];
    it->idx++;
    if (it->buf[0] >> 5 == 0x6) {
        if (it->buf[1] >> 6 != 0x2) {
            return CC_BAD_UTF8_BYTE;
        }
        *o_res = ((it->buf[0] & 0x1F) << 6) | (it->buf[1] & 0x3F);
        return CC_NO_ERROR;
    }
    if (!cc_string_iter_has_next(it)) {
        return CC_BAD_UTF8_BYTE;
    }
    it->buf[2] = it->s->buf[it->idx];
    it->idx++;
    if (it->buf[0] >> 4 == 0xE) {
        if (it->buf[1] >> 6 != 0x2 || it->buf[2] >> 6 != 0x2) {
            return CC_BAD_UTF8_BYTE;
        }
        *o_res = ((it->buf[0] & 0xF) << 12)
                | ((it->buf[1] & 0x3F) << 6)
                | (it->buf[2] & 0x3F);
        return CC_NO_ERROR;
    }
    if (!cc_string_iter_has_next(it)) {
        return CC_BAD_UTF8_BYTE;
    }
    it->buf[3] = it->s->buf[it->idx];
    it->idx++;
    if (it->buf[0] >> 3 == 0x1E) {
        if (it->buf[1] >> 6 != 0x2 || it->buf[2] >> 6 != 0x2
            || it->buf[3] >> 6 != 0x2) {
            return CC_BAD_UTF8_BYTE;
        }
        *o_res = ((it->buf[0] & 0x7) << 18)
                | ((it->buf[1] & 0x3F) << 12)
                | ((it->buf[2] & 0x3F) << 6)
                | (it->buf[3] & 0x3F);
        return CC_NO_ERROR;
    }
    return CC_BAD_UTF8_BYTE;
}


#endif /* PROJECT_CL_BUILD_STRING */
