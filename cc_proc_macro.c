#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: cc_proc_macro <input> <output>");
        return -1;
    }

    /* Temporary implementation, replace then */
    static char buffer[4096];
    sprintf(buffer, "cp \"%s\" \"%s\"", argv[1], argv[2]);
    return system(buffer);
}
