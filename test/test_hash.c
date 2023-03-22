#include "test_kit.h"
#include <cc_hash.h>
#include <cc_defs.h>
#include <cc_assert.h>
#include <cc_alloc.h>
#include <cc_memory.h>
typedef struct {
    cc_size offset;
    cc_size length;
    cc_uint8 *base;
} offset_array;

static inline void
initialize_offset_array(offset_array* array,
                        cc_size offset,
                        cc_size length) 
{
    array->length = length;
    array->base = (uint8_t *)cc_alloc(length + 128);
    cc_size current_offset = ((cc_size)array->base) % 128;
    cc_size delta = (cc_size)((offset - current_offset + 128) % 128);
    array->base = array->base + delta;
    array->offset = delta;
}

static inline void
destroy_offset_array(offset_array* array) {
    cc_free(array->base - array->offset);
}

typedef struct {
    cc_uint64 data;
} random_state;

cc_uint64 xorshift64(random_state *state)
{
    cc_uint64 x = state->data;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return state->data = x;
}


typedef cc_uint64 (* hash_func)(const void *, cc_size, cc_uint64);
void test_continuous(hash_func func) {
    random_state state;
    state.data = 0x1234567890;
    cc_uint64 expected;
    cc_uint64 actual;
    offset_array array0, array1;
    for (cc_size length = 0; length <= 16384; length ++) {
        initialize_offset_array(&array0, 0, length);
        for (cc_size i = 0; i < length; ++i) {
            array0.base[i] = (cc_uint8) xorshift64(&state);
        }
        expected = func(array0.base, length, ~length);
        for (cc_size offset = 1; offset <= 63; ++offset) {
            initialize_offset_array(&array1, offset, length);
            cc_memcpy(array1.base, array0.base, length);
            actual = func(array1.base, length, ~length);
            destroy_offset_array(&array1);
            cc_assert(actual == expected);
        }
        destroy_offset_array(&array0);
    }
}

void test1(void) {
    test_continuous(cc_stable_hash);
}

void test2(void) {
    test_continuous(cc_unstable_hash);
}


BEGIN_TEST

    AUTO_TEST_ITEM(1)
    AUTO_TEST_ITEM(2)

END_TEST
