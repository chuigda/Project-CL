
#include "test_kit.h"
#include <cc_hash.h>
#include <cc_hashmap.h>
#include <cc_memory.h>

cc_uint64 uint64_hash(void *seed, const void *key) {
  cc_uint64 seed_val = (cc_uint64)(cc_size)seed;
  if (sizeof (cc_size) < sizeof(cc_uint64)) {
    seed_val |= seed_val << 32;
  }
  return cc_unstable_hash(key, sizeof(cc_uint64), seed_val);
}

_Bool uint64_equal(void *a, void *b) {
  return *(cc_uint64 *)a == *(cc_uint64 *)b;
}

void uint64_sum_check(void *a, void *b, void *sum) {
  *(cc_uint64 *)sum += *(cc_uint64 *)a;
  cc_assert(*(cc_uint64 *)a + 1 == *(cc_uint64 *)b);
}

void test1() {
  cc_hashmap *map =
      cc_hashmap_create((cc_data_layout){.size = sizeof(cc_uint64),
                                         .alignment = sizeof(cc_uint64)},
                        (cc_data_layout){.size = sizeof(cc_uint64),
                                         .alignment = sizeof(cc_uint64)},
                        NULL, NULL, NULL, uint64_hash, uint64_equal);
  cc_uint64 limit = 100000;
  cc_uint64 sum = 0;
  cc_uint64 result = 0;
  for (cc_uint64 i = 0; i < limit; ++i) {
    cc_uint64 v = i + 1;
    sum += i;
    void *res = cc_hashmap_insert(map, &i, &v);
    cc_assert(res != NULL);
  }
  cc_assert(cc_hashmap_size(map) == limit);
  cc_hashmap_visit(map, uint64_sum_check, &result);
  cc_hashmap_destroy(map);
  cc_assert(sum == result);
}

void test2() {
  cc_uint64 limit = 100000;
  cc_hashmap *map = cc_hashmap_with_capacity(
      (cc_data_layout){.size = sizeof(cc_uint64),
                       .alignment = sizeof(cc_uint64)},
      (cc_data_layout){.size = sizeof(cc_uint64),
                       .alignment = sizeof(cc_uint64)},
      NULL, NULL, NULL, uint64_hash, uint64_equal, limit);
  cc_uint64 sum = 0;
  cc_uint64 result = 0;
  for (cc_uint64 i = 0; i < limit; ++i) {
    sum += i;
    void *res = cc_hashmap_insert(map, &i, &i);
    cc_assert(res != NULL);
  }
  for (cc_uint64 i = 0; i < limit; ++i) {
    cc_uint64 v = i + 1;
    void *res = cc_hashmap_insert(map, &i, &v);
    cc_assert(res != NULL);
  }
  for (cc_uint64 i = 0; i < limit / 2; ++i) {
    sum -= i;
    cc_hashmap_erase(map, &i);
  }
  for (cc_uint64 i = 0; i < limit / 2; ++i) {
    cc_hashmap_erase(map, &i);
  }
  cc_hashmap_visit(map, uint64_sum_check, &result);
  cc_hashmap_destroy(map);
  cc_assert(sum == result);
}

BEGIN_TEST

AUTO_TEST_ITEM(1)
AUTO_TEST_ITEM(2)

END_TEST
