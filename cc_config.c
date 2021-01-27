#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st_config_item {
    const char *item_name;
    const char *item_description;
    const char *cmd_opt;
    const char *macro_name;
    const char *src_file;
    const char *header_file;
    int enabled;
} config_item;

static config_item items[] = {
    {
        "CCLib Allocator",
        "A special dynamic allocator for low-end embedded devices",
        "--with-alloc",
        "CCLIB_ALLOC",
        "cc_alloc.c",
        "cc_alloc.h",
        0
    },
    {
        "CCLib String Enhancement Library",
        "Enhancements to string.h, includes many utilities "
        "especially slices",
        "--with-strlib",
        "CCLIB_STRLIB",
        "cc_strlib.c",
        "cc_strlib.h",
        1
    },
    {
        "CCLib Dynamic Multi-dimension Array",
        "Dynamic allocated multi dimension array",
        "--with-dmdarr",
        "CCLIB_DMDARR",
        "cc_dmdarr.c", 
        "cc_dmdarr.h", 
        1
    },
    {
        "CCLib Vector Library",
        "A continuous, O(1) random access container",
        "--with-vec",
        "CCLIB_DS_VEC",
        "cc_vec.c", 
        "cc_vec.h", 
        1
    },
    {
        "CCLib Linked List Library",
        "A doubly linked list",
        "--with-llist",
        "CCLIB_DS_LLIST",
        "cc_list.c", 
        "cc_list.h", 
        1
    },
    {
        "CCLib Intrusive List Library",
        "An intrusive linked list",
        "--with-ilist",
        "CCLIB_DS_ILIST",
        "cc_ilist.c", 
        "cc_ilist.h", 
        0
    },
    {
        "CCLib Set Library",
        "An unordered set container",
        "--with-set",
        "CCLIB_DS_SET",
        "cc_set.c", 
        "cc_set.h", 
        1
    },
    {
        "CCLib Map Library",
        "An unordered map container",
        "--with-map",
        "CCLIB_DS_MAP",
        "cc_map.c", 
        "cc_map.h", 
        1
    },
    {
        "CCLib MString Library",
        "A special arena for managing strings, use handles to represent"
        " strings, and compare strings by comparing handles.",
        "--with-mstring",
        "CCLIB_MSTRING",
        "cc_mstring.c", 
        "cc_mstring.h", 
        1
    },
    {
        "CCLib Easy Going Configuration Library",
        "A minimal structural configuration parser",
        "--with-egconf",
        "CCLIB_EGCONF",
        "cc_egconf.c", 
        "cc_egconf.h", 
        1
    },
    {
        "CCLib Argument Parser",
        "A minimal command line argument parser",
        "--with-argp",
        "CCLIB_ARGP",
        "cc_argp.c", 
        "cc_argp.h", 
        1
    },
    {
        "CCLib Test Suite",
        "A minimal test suite",
        "--with-cctest",
        "CCLIB_TEST",
        "cc_test.c", 
        "cc_test.h", 
        0
    },
    {
        "CCLib Tracing Utilities",
        "A minimal call tracing toolkit",
        "--with-trace",
        "CCLIB_TRACE",
        "cc_trace.c", 
        "cc_trace.h", 
        0
    }
};

#define COUNTOF(arr) (sizeof(arr) / sizeof(arr[0]))

static const char *help_text = 
"CCLib Configurator 1.0\n"
"Iceyey Chuigda WhiteGive <icey@icey.tech>\n"
"Interactive configurator for CCLib\n"
"\n"
"USAGE:\n"
"    %s [FLAGS]\n"
"\n"
"FLAGS:\n"
"    -i, --interactive    Use interactive configuration mode\n"
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

void interactive_config(void) {
    for (int i = 0; i < COUNTOF(items); i++) {
        fprintf(stderr, "Would you like to enable %s: %s?\n",
                items[i].item_name, items[i].item_description);
    }
}

int main(int argc, const char *argv[]) {
    if (argc == 1) {
        interactive_config();
    } else if (argc == 2) {
        if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
            const char *prog_name = 
                (argv[0] && argv[0][0]) ? argv[0] : "cc_config";
            fprintf(stderr, help_text, prog_name);
            return -1;
        } else if (!strcmp(argv[1], "-a") || !strcmp(argv[1], "--all")) {
            fprintf(stderr, "info: enabling all configuration");
            for (size_t i = 0; i < COUNTOF(items); i++) {
                items[i].enabled = 1;
            }
        } else if (!strcmp(argv[1], "-d")
                   || !strcmp(argv[1], "--default")) {
            fprintf(stderr, "info: using default configuration");
        }
    }
    return 0;
}

