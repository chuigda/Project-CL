#ifndef PROJECT_CL_DEFS_H
#define PROJECT_CL_DEFS_H

#if (defined(__has_include) && __has_include(<cc_cfg.h>)) \
    || PROJECT_CL_CFG
#   include <cc_cfg.h>
#endif

#ifndef PROJECT_CL_DONT_USE_STDDEF
#   include <stddef.h>
#endif

#ifndef PROJECT_CL_DONT_USE_STDINT
#   include <stdint.h>
#endif

#ifdef PROJECT_CL_SIZE_TYPE
typedef PROJECT_CL_SIZE_TYPE cc_size;
#else
typedef size_t cc_size;
#endif

#ifdef PROJECT_CL_ISIZE_TYPE
typedef PROJET_CL_ISIZE_TYPE cc_isize;
#else
typedef ptrdiff_t cc_isize;
#endif

#ifdef PROJECT_CL_INT8_TYPE
typedef PROJECT_CL_INT8_TYPE cc_int8;
#else
typedef int8_t cc_int8;
#endif

#ifdef PROJECT_CL_UINT8_TYPE
typedef PROJECT_CL_UINT8_TYPE cc_uint8;
#else
typedef uint8_t cc_uint8;
#endif

#ifdef PROJECT_CL_INT16_TYPE
typedef PROJECT_CL_INT16_TYPE cc_int16;
#else
typedef int16_t cc_int16;
#endif

#ifdef PROJECT_CL_UINT16_TYPE
typedef PROJECT_CL_UINT16_TYPE cc_uint16;
#else
typedef uint16_t cc_uint16;
#endif

#ifdef PROJECT_CL_INT32_TYPE
typedef PROJECT_CL_INT32_TYPE cc_int32;
#else
typedef int32_t cc_int32;
#endif

#ifdef PROJECT_CL_UINT32_TYPE
typedef PROJECT_CL_UINT32_TYPE cc_uint32;
#else
typedef uint32_t cc_uint32;
#endif

#ifdef PROJECT_CL_USE_INT64
#   ifdef PROJECT_CL_INT64_TYPE
typedef PROJECT_CL_INT64_TYPE cc_int64;
#   else
typedef int64_t cc_int64;
#   endif
#   ifdef PROJECT_CL_UINT64_TYPE
typedef PROJECT_CL_UINT64_TYPE cc_uint64;
#   else
typedef uint64_t cc_uint64;
#   endif
#endif

#ifdef PROJECT_CL_USE_INT128
#   ifdef PROJECT_CL_INT128_TYPE
typedef PROJECT_CL_INT128_TYPE cc_int128;
#   else
#       error PROJECT_CL_INT128_TYPE must be defined when enabling \
PROJECT_CL_USE_INT128
#   endif
#   ifdef PROJECT_CL_UINT128_TYPE
typedef PROJECT_CL_UINT128_TYPE cc_uint128;
#   else
#       error PROJECT_CL_UINT128_TYPE must be defined when enabling \
PROJECT_CL_USE_UINT128
#   endif
#endif

#ifdef PROJECT_CL_FP16_TYPE
typedef PROJECT_CL_FP16_TYPE cc_f16;
#endif

#ifdef PROJECT_CL_FP32_TYPE
typedef PROJECT_CL_FP32_TYPE cc_f32;
#else
typedef float cc_f32;
#endif

#ifdef PROJCT_CL_FP64_TYPE
typedef PROJECT_CL_FP64_TYPE cc_f64;
#else
typedef double cc_f64;
#endif

#ifdef PROJECT_CL_FP128_TYPE
typedef PROJECT_CL_FP128_TYPE cc_f128;
#endif

typedef _Bool (cc_pred)(void *ptr);
typedef _Bool (cc_pred2)(void *ptr, void *ctx);
typedef void (cc_dtor)(void *ptr);

#ifdef __has_builtin
#define PROJECT_CL_HAS_BUILTIN(X) __has_builtin(X)
#else
#define PROJECT_CL_HAS_BUILTIN(X) 0
#endif

#ifdef __has_attribute
#define PROJECT_CL_HAS_ATTRIBUTE(X) __has_attribute(X)
#else
#define PROJECT_CL_HAS_ATTRIBUTE(X) 0
#endif

#if PROJECT_CL_HAS_ATTRIBUTE(visibility) /* TODO: export only public APIs */
#define PROJECT_CL_EXPORT __attribute__((visbility("default")))
#else
#define PROJECT_CL_EXPORT
#endif

#endif /* PROJECT_CL_DEFS_H */
