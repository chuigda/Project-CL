#include "cc_vec.h"
#include "cc_assert.h"
#include "cc_abort.h"

#include <stdio.h>
#include <string.h>

void test1() {
    cc_vec *v = cc_vec_init(sizeof(int));
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

void test2() {
    cc_vec *v = cc_vec_init(sizeof(int));
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

void test3() {
    cc_vec *v = cc_vec_init(sizeof(int));
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

void test4() {
    cc_vec *v = cc_vec_init(sizeof(int));
    cc_assert(v);

    int arr[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    cc_vec_insert2(v, 0, arr, 8);
    cc_assert(cc_vec_size(v) == 8);

    _Bool pred(void *ptr) {
        return *(int*)ptr % 2 == 0;
    }
    cc_vec_remove_if(v, pred, NULL);
    cc_assert(cc_vec_size(v) == 4);

    int arr2[4] = { 1, 3, 5, 7 };
    cc_assert(cc_vec_size(v) == 4);
    for (cc_size i = 0; i < 4; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr2[i]);
    }

    cc_vec_destroy(v);
}

void test5() {
    cc_vec *v = cc_vec_init(sizeof(int));
    cc_assert(v);

    int arr[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    cc_vec_insert2(v, 0, arr, 8);
    cc_assert(cc_vec_size(v) == 8);

    _Bool pred(void *ptr) {
        return *(int*)ptr % 2 != 0;
    }
    cc_vec_remove_if(v, pred, NULL);
    cc_assert(cc_vec_size(v) == 4);

    int arr2[4] = { 2, 4, 6, 8 };
    cc_assert(cc_vec_size(v) == 4);
    for (cc_size i = 0; i < 4; i++) {
        int value = *(int*)cc_vec_at(v, i);
        cc_assert(value == arr2[i]);
    }

    cc_vec_destroy(v);
}

int main(int argc, char *argv[]) {
    cc_assert(argc == 2);

    if (!strcmp(argv[1], "1")) {
        test1();
    } else if (!strcmp(argv[1], "2")) {
        test2();
    } else if (!strcmp(argv[1], "3")) {
        test3();
    } else if (!strcmp(argv[1], "4")) {
        test4();
    } else if (!strcmp(argv[1], "5")) {
        test5();
    } else {
        cc_assert(0 && "should be unreachable");
    }

    return 0;
}
