#ifndef STRING_H
#define STRING_H

// Ваше определение типа для размера данных
typedef unsigned int size_t;

// --- Ваши исходные функции ---
size_t strlen(const char *s);
char* strcpy(char *dest, const char *src);
int strcmp(const char *a, const char *b);
void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
char* strstr(const char *haystack, const char *needle);

// --- Новые функции расширения ---
void* memchr(const void *s, int c, size_t n);
void* memmove(void *dest, const void *src, size_t n);
char* strcat(char *dest, const char *src);
char* strchr(const char *s, int c);

#endif
