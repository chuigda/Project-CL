#ifndef PROJECT_CL_STRING_H
#define PROJECT_CL_STRING_H

#include <cc_defs.h>

#ifndef PROJECT_CL_BUILD_STRING
#   error PROJECT_CL_BUILD_STRING not defined
#endif

#include <cc_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

struct st_cc_string;

typedef struct st_cc_string cc_string;

cc_string *cc_string_init(cc_size char_width);

#define CC_STRING_INIT(ty) cc_string_init(sizeof(ty))

cc_error cc_string_push_char(cc_string *s, void *c);

cc_error cc_string_append(cc_string *s, void *s2);

void *cc_string_at(cc_string *s, cc_size idx);

void *cc_string_tostring(cc_string *s);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_STRING_H */
