#pragma once

#include <stddef.h>

#include "types.h"

// std.c
void panic(void);
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, i32 c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
i32 memcmp(const void *s1, const void *s2, size_t n);
char *itoa(i32 num, char *str, i32 base);
char *utoa(u64 num, char *str, i32 base);

// stdstring.c
size_t strlen(const char *str);
void strncpy(char *dest, const char *src, size_t n);
void *memchr(const void *bigptr, int ch, size_t length);

// stdmath.c
i32 abs(i32 x);
