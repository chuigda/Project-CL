#include "cc_swisstable/cc_swisstable.h"
#include "cc_swisstable/safe_size.h"
#include "test_kit.h"
#include <cc_alloc.h>
#include <cc_hash.h>
#include <cc_memory.h>

typedef struct {
    cc_uint64 data;
} random_state;

cc_uint64 xorshift64(random_state *state) {
    cc_uint64 x = state->data;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return state->data = x;
}

typedef struct {
    cc_size capacity;
    cc_size size;
    cc_uint64 *data;
} brutal_force_set;

brutal_force_set bfs_create(void) {
    brutal_force_set set;
    set.capacity = 16;
    set.size = 0;
    set.data = (cc_uint64 *) cc_alloc(sizeof(cc_uint64) * set.capacity);
    return set;
}

cc_uint64 *bfs_binary_search(brutal_force_set *set, cc_uint64 target) {
    cc_size l = 0;
    cc_size r = set->size;
    while (l < r) {
        cc_size mid = (l + r) / 2;
        if (set->data[mid] == target) {
            return &set->data[mid];
        }
        if (set->data[mid] < target) {
            l = mid + 1;
        }
        if (set->data[mid] > target) {
            r = mid;
        }
    }
    return &set->data[l];
}

void bfs_grow(brutal_force_set *set) {
    cc_uint64 *data =
            (cc_uint64 * )(cc_alloc(sizeof(cc_uint64) * set->capacity * 2));
    cc_memcpy(data, set->data, sizeof(cc_uint64) * set->size);
    set->capacity *= 2;
    set->data = data;
}

void bfs_insert(brutal_force_set *set, cc_uint64 target) {
    if (set->size == set->capacity)
        bfs_grow(set);
    cc_uint64 *position = bfs_binary_search(set, target);
    if (*position != target) {
        cc_memmove(position + 1, position,
                   sizeof(cc_uint64) * (set->data + set->size - position));
        set->size++;
        *position = target;
    }
}

_Bool bfs_find(brutal_force_set *set, cc_uint64 target) {
    return target == *bfs_binary_search(set, target);
}

void bfs_erase(brutal_force_set *set, cc_uint64 target) {
    cc_uint64 *position = bfs_binary_search(set, target);
    if (*position == target) {
        cc_memmove(position, position + 1,
                   sizeof(cc_uint64) * (set->data + set->size - position - 1));
        set->size--;
    }
}

void bfs_destroy(brutal_force_set *set) { cc_free(set->data); }

#define assert_invalid_size(x) \
    cc_assert(!cc_st_safe_size_is_valid (cc_st_safe_size_create((cc_size)(x))))
#define assert_valid_size(x) \
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_create((cc_size)(x))))

void test1(void) {
    assert_invalid_size(-1);
    assert_invalid_size(-2);
    assert_invalid_size(-1024 + 33);
    assert_invalid_size(-1024 + 66);
    assert_invalid_size(cc_st_max_safe_size() + 1);
    assert_invalid_size(cc_st_max_safe_size() + 12);

    assert_valid_size(0);
    assert_valid_size(1);
    assert_valid_size(1024 + 13);
    assert_valid_size(2048 - 13);
    assert_valid_size(4096 + 1);
    assert_valid_size(8192 - 1);
    assert_valid_size(16384);
    assert_valid_size(32768 * 33);
    assert_valid_size(cc_st_max_safe_size());
    assert_valid_size(cc_st_max_safe_size() - 1);
    assert_valid_size(cc_st_max_safe_size() - 33);
    assert_valid_size(cc_st_max_safe_size() - 5261);
}

void test2(void) {
    cc_st_safe_size max = cc_st_max_safe_size();
    cc_st_safe_size half = max / 2;
    cc_st_safe_size third = max / 3;

    cc_assert(cc_st_safe_size_is_valid(half));
    cc_assert(cc_st_safe_size_is_valid(third));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_add(half, half)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_add(half, third)));
    cc_assert(cc_st_safe_size_is_valid(
            cc_st_safe_size_add(third, cc_st_safe_size_add(third, third))));

    cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_add(max, max)));
    cc_assert(!cc_st_safe_size_is_valid(
            cc_st_safe_size_add(max, cc_st_safe_size_create(1))));
    cc_assert(!cc_st_safe_size_is_valid(
            cc_st_safe_size_add(half, cc_st_safe_size_add(half, half))));
    cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_add(
            cc_st_safe_size_add(third, third), cc_st_safe_size_add(third, third))));
}

void test3(void) {
    cc_st_safe_size max = cc_st_max_safe_size();
    cc_st_safe_size half = max / 2;
    cc_st_safe_size third = max / 3;

    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(max, 1)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(1, max)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(half, 2)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(2, half)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(third, 3)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(3, third)));

    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(max, 0)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(0, max)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(half, 0)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(0, half)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(third, 0)));
    cc_assert(cc_st_safe_size_is_valid(cc_st_safe_size_mul(0, third)));

    cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_mul(max, max)));
    cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_mul(max, third)));
    cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_mul(third, max)));
    cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_mul(half, half)));
    cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_mul(half, third)));
    cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_mul(third, half)));
}

typedef enum {
    TEST_ACTION_INSERT = 0,
    TEST_ACTION_FIND = 1,
    TEST_ACTION_DELETE = 2,
    TEST_ACTION_MAX = 3
} test_action;

random_state default_random_state() {
    random_state state;
    state.data = ((cc_uint64) default_random_state) ^ 0x20E6F213CD45A379ULL;
    return state;
}

test_action next_test_action(random_state *state) {
    cc_uint64 x = xorshift64(state);
    return (test_action) (x % TEST_ACTION_MAX);
}

cc_uint64 good_hash_function(void *state, const void *key, cc_size key_size) {
    return cc_unstable_hash(key, key_size, (cc_uint64) state);
}

cc_uint64 bad_hash_function(void *state, const void *key, cc_size key_size) {
    (void) state;
    cc_uint64 x;
    cc_memcpy(&x, key, key_size < sizeof(x) ? key_size : sizeof(x));
    return x;
}

cc_uint64 really_bad_hash_function(void *state, const void *key,
                                   cc_size key_size) {
    (void) state;
    (void) key;
    (void) key_size;
    return 0;
}

cc_swisstable_hasher good_hasher(void) {
    return (cc_swisstable_hasher) {.hash = good_hash_function,
            .state = (void *) 0x123456789ABCDEF0ULL};
}

cc_swisstable_hasher bad_hasher(void) {
    return (cc_swisstable_hasher) {.hash = bad_hash_function, .state = 0};
}

cc_swisstable_hasher really_bad_hasher(void) {
    return (cc_swisstable_hasher) {.hash = really_bad_hash_function, .state = 0};
}

_Bool equality(void *a, void *b) {
    return (*(cc_uint64 *) a) == (*(cc_uint64 *) b);
}


void test_sequential_with_hasher(size_t attempts, cc_swisstable_hasher hasher) {
    cc_swisstable table_a = cc_swisstable_with_capacity(
            sizeof(cc_uint64), sizeof(cc_uint64), attempts);
    cc_swisstable table_b =
            cc_swisstable_empty(sizeof(cc_uint64), sizeof(cc_uint64));
    for (cc_uint64 i = 0; i < attempts; ++i) {
        cc_assert(cc_swisstable_insert(&table_a, &i, hasher));
        cc_assert(cc_swisstable_insert(&table_b, &i, hasher));
    }
    for (cc_uint64 i = 0; i < attempts; ++i) {
        cc_assert(*(cc_uint64 *) cc_swisstable_find(&table_a, &i, hasher,
                                                    equality) == i);
        cc_assert(*(cc_uint64 *) cc_swisstable_find(&table_b, &i, hasher,
                                                    equality) == i);
    }
    for (cc_uint64 i = attempts; i < attempts * 2; ++i) {
        cc_assert(!cc_swisstable_find(&table_a, &i, hasher, equality));
        cc_assert(!cc_swisstable_find(&table_b, &i, hasher, equality));
    }
    cc_swisstable_destroy(&table_a);
    cc_swisstable_destroy(&table_b);
}

void test4(void) {
    test_sequential_with_hasher(16384, good_hasher());
    test_sequential_with_hasher(16384, bad_hasher());
    test_sequential_with_hasher(1024, really_bad_hasher());
}

void test5(void) {

}

void test6(void) {

}


BEGIN_TEST

AUTO_TEST_ITEM(1)
AUTO_TEST_ITEM(2)
AUTO_TEST_ITEM(3)
AUTO_TEST_ITEM(4)
AUTO_TEST_ITEM(5)
AUTO_TEST_ITEM(6)

END_TEST
