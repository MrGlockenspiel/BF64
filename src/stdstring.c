#include <stddef.h>

#include "types.h"

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

void strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;
    while (i++ != n && (*dest++ = *src++))
        ;
}

void *memchr(const void *bigptr, int ch, size_t length) {
    const char *big = (const char *)bigptr;
    size_t n;
    for (n = 0; n < length; n++)
        if (big[n] == ch)
            return (void *)&big[n];
    return NULL;
}
