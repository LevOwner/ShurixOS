#include "string.h"

// --- Ваши исходные функции ---

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

char* strcpy(char *dest, const char *src) {
    char *d = dest;
    while (*src) *d++ = *src++;
    *d = 0;
    return dest;
}

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

void* memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void* memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char*)s;
    for (size_t i = 0; i < n; i++) p[i] = (unsigned char)c;
    return s;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *a = (const unsigned char*)s1;
    const unsigned char *b = (const unsigned char*)s2;
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) return (int)a[i] - (int)b[i];
    }
    return 0;
}

char* strstr(const char *haystack, const char *needle) {
    if (!*needle) return (char*)haystack;
    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && (*h == *n)) { h++; n++; }
        if (!*n) return (char*)haystack;
    }
    return 0;
}

// --- Новые функции для расширения возможностей ОС ---

/**
 * Ищет первое вхождение байта 'c' в первых 'n' байтах памяти 's'.
 * Полезна для быстрого поиска магических чисел (magic numbers) в структурах.
 */
void* memchr(const void *s, int c, size_t n) {
    const unsigned char *p = (const unsigned char*)s;
    for (size_t i = 0; i < n; i++) {
        if (p[i] == (unsigned char)c) {
            return (void*)(p + i);
        }
    }
    return 0;
}

/**
 * Безопасно копирует 'n' байт из 'src' в 'dest', даже если они перекрываются.
 * Жизненно необходима для работы аллокатора памяти ядра и сдвига очередей задач.
 */
void* memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;

    if (d < s) {
        // Если dest находится до src, копируем как обычно (слева направо)
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    } else if (d > s) {
        // Если dest после src, копируем с конца (справа налево), чтобы не затереть данные
        for (size_t i = n; i > 0; i--) d[i - 1] = s[i - 1];
    }
    return dest;
}

/**
 * Добавляет строку 'src' в конец строки 'dest'.
 * Буфер 'dest' должен иметь достаточно места. Незаменима для генерации динамических строк.
 */
char* strcat(char *dest, const char *src) {
    char *d = dest;
    while (*d) d++; // Ищем конец первой строки
    while (*src) *d++ = *src++; // Дописываем вторую строку
    *d = 0; // Выставляем терминальный ноль
    return dest;
}

/**
 * Ищет первое вхождение символа 'c' в строке 's'.
 * Часто используется при парсинге командной строки ядра или путей файловой системы.
 */
char* strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    if ((char)c == 0) return (char*)s; // Если искали конец строки
    return 0;
}
