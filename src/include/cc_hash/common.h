#ifndef PROJECT_CL_HASH_COMMON_H
#define PROJECT_CL_HASH_COMMON_H

#include <cc_defs.h>

#if defined(__has_builtin) && __has_builtin(__builtin_memcpy_inline)
#define PROJECT_CL_HASH_GENERIC_LOAD(DST, SRC) \
    __builtin_memcpy_inline((DST), (SRC), sizeof(*(DST)))
#elif defined(__has_builtin) && __has_builtin(__builtin_memcpy)
#define PROJECT_CL_HASH_GENERIC_LOAD(DST, SRC) \
    __builtin_memcpy((DST), (SRC), sizeof(*(DST)))
#else

#include <cc_memory.h>

#define PROJECT_CL_HASH_GENERIC_LOAD(DST, SRC) \
    cc_memcpy((DST), (SRC), sizeof(*(DST)))
#endif

#if defined(__SSSE3__) || defined(__ARM_FEATURE_AES)
#define PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, Y) (((X) << 4) | Y)
#define PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(X) \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x04), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x0B), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x09), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x06), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x08), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x0D), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x0F), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x05), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x0E), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x03), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x01), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x0C), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x00), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x07), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x0A), \
    PROJECT_CL_HASH_SHUFFLE_TABLE_ELEM(X, 0x02)

static _Alignas(16) cc_uint8 PROJECT_CL_HASH_SHUFFLE_TABLE[16] = {
        PROJECT_CL_HASH_SHUFFLE_TABLE_GROUP(0)
};
#endif

#endif // PROJECT_CL_HASH_COMMON_H
