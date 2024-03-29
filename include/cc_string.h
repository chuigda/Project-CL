#ifndef PROJECT_CL_STRING_H
#define PROJECT_CL_STRING_H

#include <cc_cfg.h>

#ifndef PROJECT_CL_BUILD_STRING
#   error PROJECT_CL_BUILD_STRING not defined
#endif

#include <cc_defs.h>
#include <cc_error.h>

#ifdef __cplusplus
extern "C" {
#endif

struct st_cc_string;

struct st_cc_string_iter;

typedef struct st_cc_string cc_string;

typedef struct st_cc_string_iter cc_string_iter;

cc_string *cc_string_init();

void cc_string_destroy(cc_string *s);

cc_error cc_string_push_char(cc_string *s, cc_char c);

cc_uint8 cc_string_push_rune(cc_string *s, cc_rune r);

cc_error cc_string_appendn(cc_string *s, const cc_char *s2, cc_size n);

cc_error cc_string_appends(cc_string *s, const cc_char *s2);

#define cc_string_append_strlit(s, s2) \
    cc_string_appendn((s), (s2), sizeof((s2)) - 1)

cc_char cc_string_at(cc_string *s, cc_size idx);

cc_size cc_string_len(cc_string *s);

cc_size cc_string_cap(cc_string *s);

cc_char *cc_string_cstr(cc_string *s);

cc_char *cc_string_own_cstr(cc_string *s,
                            cc_size *o_len,
                            cc_size *o_cap);

cc_string_iter *cc_string_iterator(cc_string *s);

void cc_string_iter_destroy(cc_string_iter *it);

cc_bool cc_string_iter_has_next(cc_string_iter *it);

cc_error cc_string_iter_next(cc_string_iter *it, cc_rune *o_res);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_STRING_H */
