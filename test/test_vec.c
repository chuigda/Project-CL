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

int main(int argc, char *argv[]) {
    cc_assert(argc == 2);

    if (!strcmp(argv[2], "1")) {
        test1();
    } else {
        cc_assert(0 && "should be unreachable");
    }

    return 0;
}
