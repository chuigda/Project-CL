#include "test_kit.h"

void test1(void) {
    cc_vec *v = cc_vec_init(sizeof(int), NULL);
    cc_assert(v);

    int arr[4] = { 114, 514, 1919, 810 };
    for (cc_size i = 0; i < 4; i++) {
        cc_vec_push(v, arr + i);
    }

    cc_assert(cc_vec_size(v) == 4);
    for (cc_size i = 0; i < 4; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr[i]);
    }

    cc_vec_destroy(v);
}

void test2(void) {
    cc_vec *v = cc_vec_init(sizeof(int), NULL);
    cc_assert(v);

    int arr[4] = { 114, 514, 1919, 810 };
    for (cc_size i = 0; i < 4; i++) {
        cc_vec_insert(v, 0, arr + i);
    }

    cc_assert(cc_vec_size(v) == 4);
    for (cc_size i = 0; i < 4; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr[3 - i]);
    }

    cc_vec_insert2(v, 4, arr, 4);
    cc_assert(cc_vec_size(v) == 8);
    for (cc_size i = 0; i < 4; i++) {
        int value = *(int*)cc_vec_at(v, i + 4);
        cc_assert(value == arr[i]);
    }

    int arr2[8] = { 314, 159, 26, 535, 897, 932, 384, 626 };
    cc_vec_insert2(v, 4, arr2, 8);
    cc_assert(cc_vec_size(v) == 16);
    for (cc_size i = 0; i < 8; i++) {
        int value = *(int*)cc_vec_at(v, i + 4);
        cc_assert(value == arr2[i]);
    }

    cc_vec_destroy(v);
}

void test3(void) {
    cc_vec *v = cc_vec_init(sizeof(int), NULL);
    cc_assert(v);

    int arr[8] = { 314, 159, 26, 535, 897, 932, 384, 626 };
    cc_vec_insert2(v, 0, arr, 8);
    cc_assert(cc_vec_size(v) == 8);
    
    cc_vec_remove(v, 0);
    cc_assert(cc_vec_size(v) == 7);
    for (cc_size i = 0; i < 7; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr[i + 1]);
    }

    cc_vec_remove(v, 6);
    cc_assert(cc_vec_size(v) == 6);
    for (cc_size i = 0; i < 6; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr[i + 1]);
    }

    cc_vec_remove2(v, 2, 2);
    int arr2[4] = { 159, 26, 932, 384 };
    cc_assert(cc_vec_size(v) == 4);
    for (cc_size i = 0; i < 4; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr2[i]);
    }

    cc_vec_destroy(v);
}

static inline _Bool test_vec_internal_test4_pred(void *ptr) {
    return *(int*)ptr % 2 == 0;
}

void test4(void) {
    cc_vec *v = cc_vec_init(sizeof(int), NULL);
    cc_assert(v);

    int arr[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    cc_vec_insert2(v, 0, arr, 8);
    cc_assert(cc_vec_size(v) == 8);

    cc_vec_remove_if(v, test_vec_internal_test4_pred);
    cc_assert(cc_vec_size(v) == 4);

    int arr2[4] = { 1, 3, 5, 7 };
    cc_assert(cc_vec_size(v) == 4);
    for (cc_size i = 0; i < 4; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr2[i]);
    }

    cc_vec_destroy(v);
}

static inline _Bool test_vec_internal_test5_pred(void *ptr) {
    return *(int*)ptr % 2 != 0;
}

void test5(void) {
    cc_vec *v = cc_vec_init(sizeof(int), NULL);
    cc_assert(v);

    int arr[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    cc_vec_insert2(v, 0, arr, 8);
    cc_assert(cc_vec_size(v) == 8);

    cc_vec_remove_if(v, test_vec_internal_test5_pred);
    cc_assert(cc_vec_size(v) == 4);

    int arr2[4] = { 2, 4, 6, 8 };
    cc_assert(cc_vec_size(v) == 4);
    for (cc_size i = 0; i < 4; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr2[i]);
    }

    cc_vec_destroy(v);
}

static int collected = 0;

static inline void test_vec_internal_dtor(void *ptr) {
    (void)ptr;
    collected += 1;
}

void test6(void) {
    cc_vec *v = cc_vec_init(sizeof(char), test_vec_internal_dtor);
    cc_assert(v);

    char *buf = "DEADBEEF";
    cc_vec_insert2(v, 0, buf, 8);
    cc_assert(cc_vec_size(v) == 8);

    cc_vec_destroy(v);
    cc_assert(collected == 8);
}

BEGIN_TEST

    AUTO_TEST_ITEM(1)
    AUTO_TEST_ITEM(2)
    AUTO_TEST_ITEM(3)
    AUTO_TEST_ITEM(4)
    AUTO_TEST_ITEM(5)
    AUTO_TEST_ITEM(6)

END_TEST
