#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st_config_item {
    const char *item_name;
    const char *item_description;
    const char *cmd_opt;
    const char *macro_name;
    int default_option;
} config_item;

static config_item items[] = {
    {
        "CCLib Allocator",
        "A special dynamic allocator for low-end embedded devices",
        "--with-alloc",
        "CCLIB_ALLOC",
        0
    },
    {
        "CCLib String Enhancement Library",
        "Enhancements to string.h, includes many utilities "
        "especially slices",
        "--with-strlib",
        "CCLIB_STRLIB",
        1
    },
    {
        "CCLib Dynamic Multi-dimension Array",
        "Dynamic allocated multi dimension array",
        "--with-dmdarr",
        "CCLIB_DMDARR",
        1
    },
    {
        "CCLib Vector Library",
        "A continuous, O(1) random access container",
        "--with-vec",
        "CCLIB_DS_VEC",
        1
    },
    {
        "CCLib Linked List Library",
        "A doubly linked list",
        "--with-llist",
        "CCLIB_DS_LLIST",
        1
    },
    {
        "CCLib Intrusive List Library",
        "An intrusive linked list",
        "--with-ilist",
        "CCLIB_DS_ILIST",
        0
    },
    {
        "CCLib Set Library",
        "An unordered set container",
        "--with-set",
        "CCLIB_DS_SET",
        1
    },
    {
        "CCLib Map Library",
        "An unordered map container",
        "--with-map",
        "CCLIB_DS_MAP",
        1
    },
    {
        "CCLib MString Library",
        "A special arena for managing strings, use handles to represent"
        " strings, and compare strings by comparing handles.",
        "--with-mstring",
        "CCLIB_MSTRING",
        1
    },
    {
        "CCLib Easy Going Configuration Library",
        "A minimal structural configuration parser",
        "--with-egconf",
        "CCLIB_EGCONF",
        1
    },
    {
        "CCLib Argument Parser",
        "A minimal command line argument parser",
        "--with-argp",
        "CCLIB_ARGP",
        1
    },
    {
        "CCLib Test Suite",
        "A minimal test suite",
        "--with-cctest",
        "CCLIB_TEST",
        0
    },
    {
        "CCLib Tracing Utilities",
        "A minimal call tracing toolkit",
        "--with-trace",
        "CCLIB_TRACE",
        0
    }
};

static const char *help_text = 
"CCLib Configurator 1.0\n"
"Iceyey Chuigda WhiteGive <icey@icey.tech>\n"
"Interactive configurator for CCLib\n"
"\n"
"USAGE:\n"
"    %s [FLAGS]\n"
"\n"
"FLAGS:\n"
"    -d, --default        Use the default configuration\n"
"    -a, --all            Enable all features\n"
"    --no-default         Do not enable default features\n"
"    --with-alloc         Enable dynamic memory allocator\n"
"    --with-strlib        Enable string enhancement library\n"
"    --with-dmdarr        Enable dynamic multi-dimension array\n"
"    --with-vec           Enable vector library\n"
"    --with-llist         Enable linked list library\n"
"    --with-ilist         Enable intrusive list library\n"
"    --with-set           Enable set library\n"
"    --with-map           Enable map library\n"
"    --with-mstring       Enable mstring library\n"
"    --with-egconf        Enable easy going configuration\n"
"    --with-mdb           Enable minidb library\n"
"    --with-argp          Enable commandline argument parser library\n"
"    --with-cctest        Enable CCTest library\n"
"    --with-trace         Enable tracing library\n";

int main(int argc, const char *argv[]) {
    if (argc == 2) {
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            const char *prog_name = 
                (argv[0] && argv[0][0]) ? argv[0] : "cc_config";
            fprintf(stderr, help_text, prog_name);
        }
    }
    return 0;
}

