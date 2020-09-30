#ifndef CCLIB_STRING_H
#define CCLIB_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

void *memchr(const void *str, int c, size_t n);

int memcmp();
  
int cc_strcmp(const char *str1, const char *str2);

char *cc_strcpy(char *dest, const char *from);

void cc_strcat(char *dest, const char *from);

void cc_strncat(char *dest, const char *from, cc_size_t size);

const char *cc_strstr(const char *str1, const char *str2);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
