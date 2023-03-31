#include "test_kit.h"
#include <cc_defs.h>
#include <cc_string.h>
#include <cc_alloc.h>

#include <stdio.h>

void test1(void) {
    cc_string *str = cc_string_init();
    cc_assert(str);

    const cc_char *x = "abc";
    cc_string_appends(str, x);

    cc_assert(cc_string_len(str) == 3);
    cc_size i = 0;
    for (; i < 3; i++) {
        cc_char val = cc_string_at(str, i);
        cc_assert(val == x[i]);
    }

    const cc_char *y = "def";
    cc_string_appendn(str, y, 2);
    cc_assert(cc_string_len(str) == 5);
    for (; i < 5; i++) {
        cc_assert(cc_string_at(str, i) == y[i - 3]);
    }

    cc_string_push_char(str, 'm');
    cc_assert(cc_string_len(str) == 6);
    cc_assert(cc_string_at(str, 5) == 'm');

    cc_char *s = cc_string_cstr(str);
    cc_assert(strcmp(s, "abcdem") == 0);
    cc_free(s);

    cc_string_destroy(str);
}

void test2(void) {
    cc_string *str = cc_string_init();
    cc_assert(str);

    cc_string_append_strlit(str, "只因你太美");

    cc_assert(cc_string_len(str) == sizeof("只因你太美") - 1);
    const cc_rune rs[] = {21482, 22240, 20320, 22286, 32654};
    
    cc_string_iter *it = cc_string_iterator(str);
    cc_rune r = 0;
    cc_size i = 0;
    while (cc_string_iter_has_next(it)) {
        cc_assert(cc_string_iter_next(it, &r) == CC_NO_ERROR);
        // cc_assert(r == rs[i]);
        printf("%d\n", r);
        i++;
    }
    cc_string_iter_destroy(it);

    cc_string_destroy(str);
}


BEGIN_TEST

    AUTO_TEST_ITEM(1)
    AUTO_TEST_ITEM(2)

END_TEST
