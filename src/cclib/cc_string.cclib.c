#include "cc_string.h"

CCTY(cc_ssize)
CCFN(cc_memchr) (const void *str, int c, CCTY(cc_size) n);

int
CCFN(cc_memcmp) (const void *str1, const void *str2, CCTY(cc_size) n);

CCTY(cc_size)
CCFN(cc_memcpy) (void *dest, const void *from, CCTY(cc_size) n);

CCTY(cc_ssize)
CCFN(cc_memmove) (void *dest, const void *from, CCTY(cc_size) n);

CCTY(cc_size)
CCFN(cc_strcat) (char *dest, const char *from);

CCTY(cc_size)
CCFN(cc_strncat) (char *dest, const char *from, CCTY(cc_size) n);

int
CCFN(cc_strcmp) (const char *str1, const char *str2);

int
CCFN(cc_strncmp) (const char *str1, const char *str2, CCTY(cc_size) n);

CCTY(cc_size)
CCFN(cc_strcpy) (char *dest, const char *from);

CCTY(cc_size)
CCFN(cc_strncpy) (char *dest, const char *from, CCTY(cc_size) n);

CCTY(cc_ssize)
CCFN(cc_strchr) (const char *str, int c);

CCTY(cc_ssize)
CCFN(cc_strrchr) (const char *str, int c);

CCTY(cc_ssize)
CCFN(cc_strstr) (const char *str1, const char *str2);
