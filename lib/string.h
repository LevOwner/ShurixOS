#ifndef STRING_H
#define STRING_H

typedef unsigned int size_t;

size_t strlen(const char *s);
char* strcpy(char *dest, const char *src);
char* strstr(const char *haystack, const char *needle);
int strcmp(const char *a, const char *b);
void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
char* strstr(const char *haystack, const char *needle);

#endif