#include <cc_vec.h>
#include <vktest.h>

static void test_cc_vec_push_back(void);
static void test_cc_vec_push_front(void);
static void test_cc_vec_traverse(void);
static void test_cc_vec_insert(void);
static void test_cc_vec_remove(void);
static void test_cc_vec_remove_n(void);
static void test_cc_vec_remove_n2(void);

int main() {
    VK_TEST_BEGIN;

    test_cc_vec_push_back();
    test_cc_vec_push_front();
    test_cc_vec_traverse();
    test_cc_vec_insert();
    test_cc_vec_remove();
    test_cc_vec_remove_n();
    test_cc_vec_remove_n2();

    VK_TEST_END;
}

static void test_cc_vec_push_back(void) {
    VK_TEST_SECTION_BEGIN("CC_VEC_PUSH_BACK");
    cc_vec vec;
    cc_vec_init(&vec, sizeof(int));
    VK_ASSERT(cc_vec_empty(&vec));

    for (int i = 0; i < 10; i++) {
        cc_vec_push_back(&vec, &i);
        VK_ASSERT_EQUALS(cc_vec_len(&vec), (cc_size)(i + 1));
    }

    for (int i = 9; i >= 0; i--) {
        VK_ASSERT_EQUALS(*(int*)cc_vec_back(&vec), i);
        cc_vec_pop_back(&vec);
        VK_ASSERT_EQUALS(cc_vec_len(&vec), (cc_size)i);
    }

    VK_ASSERT(cc_vec_empty(&vec));
    cc_vec_destroy(&vec);
    VK_TEST_SECTION_END("CC_VEC_PUSH_BACK");
}

static void test_cc_vec_push_front(void) {
    VK_TEST_SECTION_BEGIN("CC_VEC_PUSH_FRONT");
    cc_vec vec;
    cc_vec_init(&vec, sizeof(int));

    for (int i = 0; i < 10; i++) {
        cc_vec_push_front(&vec, &i);
        VK_ASSERT_EQUALS(cc_vec_len(&vec), (cc_size)(i + 1));
    }

    for (int i = 9; i >= 0; i--) {
        VK_ASSERT_EQUALS(*(int*)cc_vec_front(&vec), i);
        cc_vec_pop_front(&vec);
        VK_ASSERT_EQUALS(cc_vec_len(&vec), (cc_size)i);
    }

    VK_ASSERT(cc_vec_empty(&vec));
    cc_vec_destroy(&vec);
    VK_TEST_SECTION_BEGIN("CC_VEC_PUSH_FRONT");
}

static void test_cc_vec_traverse(void) {
    VK_TEST_SECTION_BEGIN("CC_VEC_TRAVERSE");
    cc_vec vec;
    cc_vec_init(&vec, sizeof(int));

    for (int i = 0; i < 10; i++) {
        cc_vec_push_back(&vec, &i);
    }
    VK_ASSERT_EQUALS(cc_vec_len(&vec), 10);

    for (int i = 0; i < 10; i++) {
        VK_ASSERT_EQUALS(*(int*)cc_vec_nth(&vec, i), i);
    }

    cc_vec_destroy(&vec);
    VK_TEST_SECTION_END("CC_VEC_TRAVERSE");
}

static void test_cc_vec_insert(void) {
    VK_TEST_SECTION_BEGIN("CC_VEC_INSERT");
    cc_vec vec;
    cc_vec_init(&vec, sizeof(int));

    for (int i = 0; i < 5; i++) {
        cc_vec_push_back(&vec, &i);
    }

    int data = 114;
    cc_vec_insert(&vec, 0, &data);
    data = 514;
    cc_vec_insert(&vec, 3, &data);
    data = 1919810;
    cc_vec_insert(&vec, 6, &data);
    VK_ASSERT_EQUALS(cc_vec_len(&vec), 8);
    int values[] = {114, 0, 1, 514, 2, 3, 1919810, 4};
    for (int i = 0; i < 8; i++) {
        VK_ASSERT_EQUALS(*(int*)cc_vec_nth(&vec, i), values[i]);
    }

    cc_vec_destroy(&vec);
    VK_TEST_SECTION_END("CC_VEC_INSERT");
}

static void test_cc_vec_remove(void) {
    VK_TEST_SECTION_BEGIN("CC_VEC_REMOVE");
    cc_vec vec;
    cc_vec_init(&vec, sizeof(int));

    for (int i = 0; i < 10; i++) {
        cc_vec_push_back(&vec, &i);
    }

    cc_vec_destroy(&vec);
    VK_TEST_SECTION_END("CC_VEC_REMOVE");
}

static void test_cc_vec_remove_n(void) {
    VK_TEST_SECTION_BEGIN("CC_VEC_REMOVE_N");


    VK_TEST_SECTION_END("CC_VEC_REMOVE_N");
}

static void test_cc_vec_remove_n2(void) {
    VK_TEST_SECTION_BEGIN("CC_VEC_REMOVE_N2");


    VK_TEST_SECTION_END("CC_VEC_REMOVE_N2");
}
