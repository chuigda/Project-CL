#include <cc_cfg.h>

#ifdef PROJECT_CL_BUILD_STRING

#include <cc_string.h>
#include <cc_vec.h>
#include <cc_alloc.h>
#include <cc_error.h>

struct st_cc_string {
    cc_vec* vec;
    cc_error (*append_func)(cc_string *s, void* s2);
};

#define CC_STRING_APPENDN(n, ty) \
    cc_error cc_string_append##n(cc_string *s, ty *s2) { \
        cc_error err; \
        while (*s2 != 0) { \
            err = cc_vec_push(s->vec, s2); \
            if (err != CC_NO_ERROR) { \
                return err; \
            } \
            s2++; \
        } \
        return CC_NO_ERROR; \
    }

CC_STRING_APPENDN(8,    cc_char)
CC_STRING_APPENDN(16,   cc_char16)
CC_STRING_APPENDN(32,   cc_char32)

cc_string *cc_string_init(cc_size char_width) {
    cc_vec* vec = cc_vec_init(char_width, NULL);
    cc_string* str = cc_alloc(sizeof(cc_string));
    if (str) {
        str->vec = vec;
        switch (char_width)
        {
        case sizeof(cc_char):
            str->append_func = cc_string_append8;
            break;
        case sizeof(cc_char16):
            str->append_func = cc_string_append16;
            break;
        case sizeof(cc_char32):
            str->append_func = cc_string_append32;
            break;
        default:
            cc_free(str);
            return NULL;
        }
    }
    return str;
}

cc_error cc_string_push_char(cc_string *s, void *c) {
    return cc_vec_push(s->vec, c);
}

cc_error cc_string_append(cc_string *s, void *s2) {
    return s->append_func(s, s2);
}

void *cc_string_at(cc_string *s, cc_size idx) {
    return cc_vec_at(s->vec, idx);
}

void *cc_string_tostring(cc_string *s) {
    cc_size vec_size = cc_vec_size(s->vec);
    cc_size char_width = cc_vec_item_size(s->vec);
    void *buf = cc_alloc((vec_size + 1) * char_width);
    cc_vec_copy(s->vec, buf);
    cc_char *ptr = ((cc_char*)buf) + (vec_size * char_width);
    for (cc_size i = 0; i < char_width; i++) {
        ptr[i] = 0;
    }
    return buf;
}


#endif /* PROJECT_CL_BUILD_STRING */
