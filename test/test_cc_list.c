#include <cc_list.h>
#include <vktest.h>

static void test_cc_list_push_front(void);
static void test_cc_list_push_back(void);
static void test_cc_list_iter(void);
static void test_cc_list_insert_before(void);
static void test_cc_list_insert_before2(void);
static void test_cc_list_remove(void);
static void test_cc_list_remove_range(void);
static void test_cc_list_remove_range2(void);

int main() {
    VK_TEST_BEGIN;

    test_cc_list_push_front();
    test_cc_list_push_back();
    test_cc_list_iter();
    test_cc_list_insert_before();
    test_cc_list_insert_before2();
    test_cc_list_remove();
    test_cc_list_remove_range();
    test_cc_list_remove_range2();

    VK_TEST_END;
}

static void test_cc_list_push_front(void) {
    VK_TEST_SECTION_BEGIN("CC_LIST_PUSH_POP_FRONT");
    cc_list list;
    cc_list_init(&list, sizeof(int));
    VK_ASSERT(cc_list_empty(&list));

    VK_ASSERT_EQUALS(cc_list_len(&list), 0);

    for (int i = 0; i < 4; i++) {
        cc_list_push_front(&list, &i);
        VK_ASSERT_EQUALS(cc_list_len(&list), (cc_size)(i + 1));
    }

    for (int i = 3; i >= 0; i--) {
        int *data = (int*)cc_list_front(&list);
        VK_ASSERT_EQUALS(*data, i);
        cc_list_pop_front(&list);
        VK_ASSERT_EQUALS(cc_list_len(&list), (cc_size)i);
    }

    VK_ASSERT(cc_list_empty(&list));
    cc_list_destroy(&list);
    VK_TEST_SECTION_END("CC_LIST_PUSH_POP_FRONT");
}

static void test_cc_list_push_back(void) {
    VK_TEST_SECTION_BEGIN("CC_LIST_PUSH_POP_BACK");
    cc_list list;
    cc_list_init(&list, sizeof(int));
    VK_ASSERT(cc_list_empty(&list));

    VK_ASSERT_EQUALS(cc_list_len(&list), 0);

    for (int i = 0; i < 4; i++) {
        cc_list_push_back(&list, &i);
        VK_ASSERT_EQUALS(cc_list_len(&list), (cc_size)(i + 1));
    }

    for (int i = 3; i >= 0; i--) {
        int *data = (int*)cc_list_back(&list);
        VK_ASSERT_EQUALS(*data, i);
        cc_list_pop_back(&list);
        VK_ASSERT_EQUALS(cc_list_len(&list), (cc_size)i);
    }

    VK_ASSERT(cc_list_empty(&list));
    cc_list_destroy(&list);
    VK_TEST_SECTION_END("CC_LIST_PUSH_POP_BACK")
}

static void test_cc_list_iter(void) {
    VK_TEST_SECTION_BEGIN("CC_LIST_ITERATOR");
    cc_list list;
    cc_list_init(&list, sizeof(int));

    for (int i = 0; i < 10; i++) {
        cc_list_push_back(&list, &i);
    }
    VK_ASSERT_EQUALS(cc_list_len(&list), 10);

    int n = 0;
    for (cc_list_iter iter = cc_list_begin(&list);
         !cc_list_iter_eq(iter, cc_list_end(&list));
         iter = cc_list_iter_next(iter)) {
        VK_ASSERT_EQUALS(*(int*)cc_list_iter_deref(iter), n);
        n++;
    }

    cc_list_destroy(&list);
    VK_TEST_SECTION_END("CC_LIST_ITERATOR");
}

static void test_cc_list_insert_before(void) {
    VK_TEST_SECTION_BEGIN("CC_LIST_INSERT_BEFORE");
    cc_list list;
    cc_list_init(&list, sizeof(int));

    cc_list_iter back = cc_list_end(&list);
    for (int i = 0; i < 10; i++) {
        cc_list_insert(&list, back, &i);
        VK_ASSERT_EQUALS(*(int*)cc_list_back(&list), i);
        VK_ASSERT_EQUALS(cc_list_len(&list), (cc_size)(i + 1));
    }

    {
        int n = 0;
        for (cc_list_iter iter = cc_list_begin(&list);
             !cc_list_iter_eq(iter, cc_list_end(&list));
             iter = cc_list_iter_next(iter)) {
            VK_ASSERT_EQUALS(*(int*)cc_list_iter_deref(iter), n);
            n++;
        }
    }

    {
        int n = 9;
        for (cc_list_iter iter = cc_list_iter_prev(cc_list_end(&list));
             !cc_list_iter_eq(iter, cc_list_begin(&list));
             iter = cc_list_iter_prev(iter)) {
            VK_ASSERT_EQUALS(*(int*)cc_list_iter_deref(iter), n);
            n--;
        }
    }

    cc_list_destroy(&list);
    VK_TEST_SECTION_END("CC_LIST_INSERT_BEFORE");
}

static void test_cc_list_insert_before2(void) {
    VK_TEST_SECTION_BEGIN("CC_LIST_INSERT_BEFORE2");
    cc_list list;
    cc_list_init(&list, sizeof(int));

    for (int i = 1; i <= 5; i++) {
        cc_list_push_back(&list, &i);
    }

    {
        int value_to_find = 3;
        cc_list_iter iter = cc_list_find_value(&list, &value_to_find);
        VK_ASSERT(!cc_list_iter_eq(iter, cc_list_end(&list)));

        int value = 12;
        cc_list_insert(&list, iter, &value);
        value = 14;
        cc_list_insert(&list, iter, &value);
    }
    VK_ASSERT_EQUALS(cc_list_len(&list), 7);

    {
        int values[] = {1, 2, 12, 14, 3, 4, 5};
        cc_list_iter iter = cc_list_begin(&list);
        for (int i = 0; i < 7; i++) {
            VK_ASSERT_EQUALS(*(int*)cc_list_iter_deref(iter), values[i]);
            iter = cc_list_iter_next(iter);
        }
    }

    cc_list_destroy(&list);
    VK_TEST_SECTION_END("CC_LIST_INSERT_BEFORE2");
}

static void test_cc_list_remove(void) {
    VK_TEST_SECTION_BEGIN("CC_LIST_REMOVE");
    cc_list list;
    cc_list_init(&list, sizeof(int));

    {
        int org_values[] = {114, 514, 1919, 810, 138, 38138};
        for (int i = 0; i < 6; i++) {
            cc_list_push_back(&list, org_values + i);
        }

        cc_list_iter iter = cc_list_find_value(&list, org_values + 2);
        VK_ASSERT(!cc_list_iter_eq(iter, cc_list_end(&list)));
        cc_list_remove(&list, iter);

        iter = cc_list_find_value(&list, org_values);
        VK_ASSERT(!cc_list_iter_eq(iter, cc_list_end(&list)));
        cc_list_remove(&list, iter);

        iter = cc_list_find_value(&list, org_values + 5);
        VK_ASSERT(!cc_list_iter_eq(iter, cc_list_end(&list)));
        cc_list_remove(&list, iter);

        int values[] = {514, 810, 138};
        VK_ASSERT_EQUALS(cc_list_len(&list), 3);

        iter = cc_list_begin(&list);
        for (int i = 0; i < 3; i++) {
            VK_ASSERT_EQUALS(*(int*)cc_list_iter_deref(iter), values[i]);
            iter = cc_list_iter_next(iter);
        }
    }

    cc_list_destroy(&list);
    VK_TEST_SECTION_END("CC_LIST_REMOVE");
}

static void test_cc_list_remove_range(void) {
    VK_TEST_SECTION_BEGIN("CC_LIST_REMOVE_RANGE");
    cc_list list;
    cc_list_init(&list, sizeof(int));

    int org_values[] = {114, 514, 1919, 810, 138, 38138};
    for (int i = 0; i < 6; i++) {
        cc_list_push_back(&list, org_values + i);
    }

    cc_list_iter iter1 = cc_list_find_value(&list, org_values + 1);
    cc_list_iter iter2 = cc_list_find_value(&list, org_values + 5);
    VK_ASSERT(!cc_list_iter_eq(iter1, cc_list_end(&list)));
    VK_ASSERT(!cc_list_iter_eq(iter2, cc_list_end(&list)));
    cc_list_remove_range(&list, iter1, iter2);

    VK_ASSERT_EQUALS(cc_list_len(&list), 2);
    VK_ASSERT_EQUALS(*(int*)cc_list_front(&list), 114);
    VK_ASSERT_EQUALS(*(int*)cc_list_back(&list), 38138);

    cc_list_destroy(&list);
    VK_TEST_SECTION_END("CC_LIST_REMOVE_RANGE");
}

static void test_cc_list_remove_range2(void) {
    VK_TEST_SECTION_BEGIN("CC_LIST_REMOVE_RANGE2");
    cc_list list;
    cc_list_init(&list, sizeof(int));

    int org_values[] = {114, 514, 1919, 810, 138, 38138};
    for (int i = 0; i < 6; i++) {
        cc_list_push_back(&list, org_values + i);
    }

    cc_list_iter iter1 = cc_list_begin(&list);
    cc_list_iter iter2 = cc_list_end(&list);
    cc_list_remove_range(&list, iter1, iter2);

    VK_ASSERT_EQUALS(cc_list_len(&list), 0);

    cc_list_destroy(&list);
    VK_TEST_SECTION_END("CC_LIST_REMOVE_RANGE2");
}
