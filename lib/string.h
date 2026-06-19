#ifndef STRING_H
#define STRING_H

// Свой тип size_t (без stddef)
typedef unsigned int size_t;

// Типы из stdint (свои)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

size_t strlen(const char *s);
char* strcpy(char *dest, const char *src);
int strcmp(const char *a, const char *b);
void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif