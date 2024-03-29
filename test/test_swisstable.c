#include "cc_swisstable/cc_swisstable.h"
#include "cc_swisstable/safe_size.h"
#include "test_kit.h"
#include <cc_alloc.h>
#include <cc_hash.h>
#include <cc_memory.h>
#include <cc_vec.h>

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
  cc_vec * vector;
} brutal_force_set;

brutal_force_set bfs_create(void) {
  brutal_force_set set;
  set.vector = cc_vec_init(sizeof(cc_uint64), NULL);
  return set;
}

_Bool bfs_find(brutal_force_set *set, cc_uint64 target) {
  cc_size size = cc_vec_size(set->vector);
  if (!size) {
    return 0;
  }
  const uint64_t* __restrict buf = (uint64_t*)cc_vec_front(set->vector);
  for (cc_size i = 0; i < size; ++i) {
    if (buf[i] == target) {
      return 1;
    }
  }
  return 0;
}

void bfs_insert(brutal_force_set *set, cc_uint64 target) {
  if (!bfs_find(set, target)) {
    cc_vec_push(set->vector, &target);
  }
}

_Bool remove_check(void * x, void * y) {
  return *(cc_uint64 *)x = *(cc_uint64 *)y;
}

void bfs_erase(brutal_force_set *set, cc_uint64 target) {
  cc_vec_remove_if2(set->vector, remove_check, &target);
}

cc_uint64 * bfs_at(brutal_force_set *set, cc_size i) {
  return (cc_uint64 *)cc_vec_at(set->vector, i);
}

void bfs_destroy(brutal_force_set *set) { cc_vec_destroy(set->vector); }

#define assert_invalid_size(x)                                                 \
  cc_assert(!cc_st_safe_size_is_valid(cc_st_safe_size_create((cc_size)(x))))
#define assert_valid_size(x)                                                   \
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

random_state default_random_state(void) {
  random_state state;
  state.data = ((cc_uint64)(cc_size)default_random_state);
  if (sizeof (cc_size) < sizeof(cc_uint64)) {
    state.data |= state.data << 32;
  }
  state.data ^= 2370766716244425513;
  return state;
}

test_action next_test_action(random_state *state) {
  cc_uint64 x = xorshift64(state);
  return (test_action)(x % TEST_ACTION_MAX);
}

cc_uint64 good_hash_function(void *state, const void *key) {
  cc_uint64 seed = (cc_uint64)(cc_size)state;
  if (sizeof (cc_size) < sizeof(cc_uint64)) {
    seed |= seed << 32;
  }
  return cc_unstable_hash(key, sizeof(cc_uint64), seed);
}

cc_uint64 bad_hash_function(void *state, const void *key) {
  (void)state;
  cc_uint64 x;
  cc_memcpy(&x, key, sizeof(x));
  return x;
}

cc_uint64 really_bad_hash_function(void *state, const void *key) {
  (void)state;
  (void)key;
  return 0;
}

cc_swisstable_hasher good_hasher(void) {
  return (cc_swisstable_hasher){.hash = good_hash_function,
                                .state = (void *)0x123456789ABCDEF0ULL};
}

cc_swisstable_hasher bad_hasher(void) {
  return (cc_swisstable_hasher){.hash = bad_hash_function, .state = 0};
}

cc_swisstable_hasher really_bad_hasher(void) {
  return (cc_swisstable_hasher){.hash = really_bad_hash_function, .state = 0};
}

_Bool equality(void *a, void *b) {
  return (*(cc_uint64 *)a) == (*(cc_uint64 *)b);
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
    cc_assert(
        *(cc_uint64 *)cc_swisstable_find(&table_a, &i, hasher, equality) == i);
    cc_assert(
        *(cc_uint64 *)cc_swisstable_find(&table_b, &i, hasher, equality) == i);
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

void random_walk(cc_size attempts, cc_swisstable_hasher hasher) {
  brutal_force_set set = bfs_create();
  cc_swisstable table =
      cc_swisstable_empty(sizeof(cc_uint64), sizeof(cc_uint64));
  random_state state = default_random_state();
  for (cc_size i = 0; i < attempts; ++i) {
    switch (next_test_action(&state)) {
    case TEST_ACTION_INSERT: {
      cc_uint64 x = xorshift64(&state);
      bfs_insert(&set, x);
      if (!cc_swisstable_find(&table, &x, hasher, equality)) {
        void *result = cc_swisstable_insert(&table, &x, hasher);
        cc_assert(*(cc_uint64 *)result == x);
      }
      cc_assert(
          *(cc_uint64 *)cc_swisstable_find(&table, &x, hasher, equality) == x);
      break;
    }
    case TEST_ACTION_FIND: {
      if (!cc_vec_size(set.vector))
        continue;
      cc_uint64 x = xorshift64(&state);
      cc_assert(((_Bool)cc_swisstable_find(&table, &x, hasher, equality)) ==
                bfs_find(&set, x));
      cc_size idx = x % cc_vec_size(set.vector);
      cc_assert(*(cc_uint64 *)cc_swisstable_find(
                    &table,
                    bfs_at(&set, idx),
                    hasher, equality) == *bfs_at(&set, idx));
      break;
    }
    case TEST_ACTION_DELETE: {
      if (!cc_vec_size(set.vector))
        continue;
      cc_uint64 x = xorshift64(&state);
      cc_size idx = (cc_size)x % cc_vec_size(set.vector);
      cc_uint64 target = *bfs_at(&set, idx);
      bfs_erase(&set, target);
      cc_assert(*(cc_uint64 *)cc_swisstable_find(&table, &target, hasher,
                                                 equality) == target);
      cc_swisstable_erase(
          &table, cc_swisstable_find(&table, &target, hasher, equality));
      cc_assert(!cc_swisstable_find(&table, &target, hasher, equality));
      break;
    }
    default:
      continue;
    }
  }
  for (cc_size i = 0; i < cc_vec_size(set.vector); ++i) {
    cc_assert(*(cc_uint64 *)cc_swisstable_find(
                  &table, bfs_at(&set, i), hasher, equality)
              == *bfs_at(&set, i));
  }
  for (cc_size i = 0; i < attempts; ++i) {
    cc_uint64 x = xorshift64(&state);
    cc_assert(((_Bool)cc_swisstable_find(&table, &x, hasher, equality)) ==
              bfs_find(&set, x));
  }
  cc_swisstable_destroy(&table);
  bfs_destroy(&set);
}

void rehash_test(cc_swisstable_hasher hasher) {
  cc_swisstable table =
      cc_swisstable_with_capacity(sizeof(cc_uint64), sizeof(cc_uint64), 512);
  cc_uint64 i = 0;
  for (; i < 512; ++i) {
    if (table.growth_left != 0) {
      cc_swisstable_insert(&table, &i, hasher);
    } else {
      break;
    }
  }

  cc_uint64 n = i;

  for (i = 0; i < 128; ++i) {
    cc_swisstable_erase(&table,
                        cc_swisstable_find(&table, &i, hasher, equality));
  }

  for (i = 1000; i < 1128; ++i) {
    cc_swisstable_insert(&table, &i, hasher);
  }

  for (i = 0; i < 128; ++i) {
    cc_assert(!cc_swisstable_find(&table, &i, hasher, equality));
  }

  for (i = n; i < 512; ++i) {
    cc_assert(*(cc_uint64 *)cc_swisstable_find(&table, &i, hasher, equality) ==
              i);
  }

  for (i = 1000; i < 1128; ++i) {
    cc_assert(*(cc_uint64 *)cc_swisstable_find(&table, &i, hasher, equality) ==
              i);
  }

  cc_swisstable_destroy(&table);
}

void test5(void) {
  random_walk(32768, good_hasher());
  random_walk(32768, bad_hasher());
  random_walk(4096, really_bad_hasher());
}

void test6(void) {
  rehash_test(good_hasher());
  rehash_test(bad_hasher());
  rehash_test(really_bad_hasher());
}

void iterator_test(cc_size size, cc_swisstable_hasher hasher) {
  cc_swisstable table =
      cc_swisstable_empty(sizeof(cc_uint64), sizeof(cc_uint64));
  brutal_force_set set = bfs_create();
  random_state state = default_random_state();
  for (cc_size i = 0; i < size; ++i) {
    cc_uint64 x = xorshift64(&state);
    if (!cc_swisstable_find(&table, &x, hasher, equality)) {
      cc_swisstable_insert(&table, &x, hasher);
      bfs_insert(&set, x);
    }
  }
  cc_size i = 0;
  cc_swisstable_iter iter = cc_swisstable_create_iter(&table);
  void *ptr = cc_swisstable_iter_next(&iter);
  while (ptr) {
    i += 1;
    cc_assert(bfs_find(&set, *(cc_uint64 *)ptr));
    ptr = cc_swisstable_iter_next(&iter);
  }
  cc_assert(i == cc_vec_size(set.vector));
  bfs_destroy(&set);
  cc_swisstable_destroy(&table);
}

void test7(void) {
  iterator_test(32768, good_hasher());
  iterator_test(32768, bad_hasher());
  iterator_test(1024, really_bad_hasher());
}

BEGIN_TEST

AUTO_TEST_ITEM(1)
AUTO_TEST_ITEM(2)
AUTO_TEST_ITEM(3)
AUTO_TEST_ITEM(4)
AUTO_TEST_ITEM(5)
AUTO_TEST_ITEM(6)
AUTO_TEST_ITEM(7)
END_TEST
