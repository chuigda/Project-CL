#include "cc_string.h"

CCTY(cc_ssize) cc_memchr(const void *str, int c, CCTY(cc_size) n);

int cc_memcmp(const void *str1, const void *str2, CCTY(cc_size) n);

CCTY(cc_size) cc_memcpy(void *dest, const void *from, CCTY(cc_size) n);

CCTY(cc_ssize) cc_memmove(void *dest, const void *from, CCTY(cc_size) n);

CCTY(cc_size) cc_strcat(char *dest, const char *from);

CCTY(cc_size) cc_strncat(char *dest, const char *from, CCTY(cc_size) n);

int cc_strcmp(const char *str1, const char *str2);

int cc_strncmp(const char *str1, const char *str2, CCTY(cc_size) n);

CCTY(cc_size) cc_strcpy(char *dest, const char *from);

CCTY(cc_size) cc_strncpy(char *dest, const char *from, CCTY(cc_size) n);

CCTY(cc_ssize) cc_strchr(const char *str, int c);

CCTY(cc_ssize) cc_strrchr(const char *str, int c);

CCTY(cc_ssize) cc_strstr(const char *str1, const char *str2);
