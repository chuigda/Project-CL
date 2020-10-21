#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum e_prebuilt_config {
    PBC_ALL_DEFAULT,
    PBC_ALL_STD,
    PBC_ALL_CCLIB
} prebuilt_config;

int main(int argc, const char *argv[]) {
    if (argc == 2) {
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        }
    }
    return 0;
}
