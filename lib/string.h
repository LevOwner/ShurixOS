#ifndef STRING_H
#define STRING_H

// ============================================================
// СТАНДАРТНЫЕ ТИПЫ
// ============================================================

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;

typedef unsigned int       size_t;

// ============================================================
// СТРОКОВЫЕ ФУНКЦИИ
// ============================================================

size_t strlen(const char *s);
char* strcpy(char *dest, const char *src);
char* strstr(const char *haystack, const char *needle);
int strcmp(const char *a, const char *b);
void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif