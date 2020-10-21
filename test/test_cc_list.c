#include <cc_list.h>
#include <vktest.h>

static void test_cc_list_push_front(void);
static void test_cc_list_push_back(void);
static void test_cc_list_iter(void);
static void test_cc_list_insert_before(void);
static void test_cc_list_insert_before2(void);

int main() {
    VK_TEST_BEGIN;

    test_cc_list_push_front();
    test_cc_list_push_back();
    test_cc_list_iter();
    test_cc_list_insert_before();
    test_cc_list_insert_before2();

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



    VK_TEST_SECTION_END("CC_LIST_INSERT_BEFORE2");
}
