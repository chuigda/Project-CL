#ifndef PROJECT_CL_HASH_COMMON_H
#define PROJECT_CL_HASH_COMMON_H

#include <cc_defs.h>

#if defined(__has_builtin) && __has_builtin(__builtin_memcpy_inline)
#define CC_HASH_GENERIC_LOAD(DST, SRC) __builtin_memcpy_inline((DST), (SRC), sizeof(*(DST)))
#elif defined(__has_builtin) && __has_builtin(__builtin_memcpy)
#define CC_HASH_GENERIC_LOAD(DST, SRC) __builtin_memcpy((DST), (SRC), sizeof(*(DST)))
#else

#include <cc_memory.h>

#define CC_HASH_GENERIC_LOAD(DST, SRC) cc_memcpy((DST), (SRC), sizeof(*(DST)))
#endif

#define CC_HASH_SHUFFLE_TABLE_ELEM(X, Y) (((X) << 4) | Y)
#define CC_HASH_SHUFFLE_TABLE_GROUP(X) \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x05), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x0F), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x0D), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x08), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x06), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x09), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x0B), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x04), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x02), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x0A), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x07), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x00), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x0C), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x01), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x03), \
    CC_HASH_SHUFFLE_TABLE_ELEM(X, 0x0E)

static _Alignas(16) cc_uint8 CC_HASH_SHUFFLE_TABLE[16] = {
        CC_HASH_SHUFFLE_TABLE_GROUP(0)
};

#endif // PROJECT_CL_HASH_COMMON_H