#ifndef PROJECT_CL_TEST_KIT_H
#define PROJECT_CL_TEST_KIT_H

#include "cc_vec.h"
#include "cc_assert.h"
#include <string.h>

#define AUTO_TEST_ITEM(ID) \
    { \
        if (!strcmp(argv[1], #ID)) { \
            test##ID(); \
            return 0; \
        } \
    }

#define BEGIN_TEST \
    int main(int argc, char *argv[]) { \
        cc_assert(argc == 2);

#define END_TEST \
        cc_assert(0 && "should be unreachable"); \
    }

#endif /* PROJECT_CL_TEST_KIT_H */
