#ifndef PROJECT_CL_PL5_H
#define PROJECT_CL_PL5_H

#include <cc_defs.h>
#include <cc_error.h>

#if !defined(PROJECT_CL_IN_BUILD) && !defined(PROJECT_CL_BUILD_PL5)
#   error PROJECT_CL_BUILD_PL5 not defined
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct st_cc_pl5_context;
struct st_cc_pl5_program;

typedef struct st_cc_pl5_context cc_pl5_context;
typedef struct st_cc_pl5_program cc_pl5_program;

typedef enum {
    CC_PL5_NONE = 0,
    CC_PL5_INT = 1,
    CC_PL5_FLOAT = 2
} cc_pl5_type;

typedef cc_uint32 (cc_pl5_user_fn)(cc_uint32 *values);

cc_pl5_context *cc_pl5_init();

void cc_pl5_destroy(cc_pl5_context *cx);

cc_error cc_pl5_define_userdata(cc_pl5_compile *cx,
                                char const *name,
                                cc_pl5_type type,
                                cc_size offset);

cc_error cc_pl5_define_userdata2(cc_pl5_context *cx,
                                 char const **names,
                                 cc_pl5_type *types,
                                 cc_size *offsets);

cc_error cc_pl5_define_userfn(cc_pl5_context *cx,
                              char const *fn_name,
                              cc_pl5_type *param_types,
                              cc_pl5_type ret_type,
                              cc_pl5_user_fn *fn_ptr);

typedef struct {
    cc_error err;
    cc_uint32 code;
    char const *file;
    cc_uint32 line;
    cc_uint32 col;
    char description[];
} cc_pl5_error;

cc_pl5_error *cc_pl5_compile(cc_pl5_context *cx,
                             char const *file_name,
                             char *source);

cc_pl5_error *cc_pl5_finalize(cc_pl5_context *cx,
                              cc_pl5_program **p_program);

void cc_pl5_program_deinit(cc_pl5_program *program);

cc_pl5_error *cc_pl5_run(cc_pl5_program *program, void *user_data);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PROJECT_CL_PL5_H */
