#include <stddef.h>

#include "serial.h"
#include "types.h"

void panic(void) {
    com_printf("kernel panic\r\n");
    __asm__ __volatile__("cli");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}

void *memcpy(void *dest, const void *src, size_t n) {
    u8 *pdest = (u8 *)dest;
    const u8 *psrc = (const u8 *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, i32 c, size_t n) {
    u8 *p = (u8 *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (u8)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    u8 *pdest = (u8 *)dest;
    const u8 *psrc = (const u8 *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}

i32 memcmp(const void *s1, const void *s2, size_t n) {
    const u8 *p1 = (const u8 *)s1;
    const u8 *p2 = (const u8 *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

void reverse(char *str, i32 length) {
    i32 start = 0;
    i32 end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}

char *itoa(i32 num, char *str, i32 base) {
    i32 i = 0;
    i8 is_n = 0;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // in standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        is_n = 1;
        num = -num;
    }

    // process individual digits
    while (num != 0) {
        i32 rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // if number is negative, append '-'
    if (is_n) {
        str[i++] = '-';
    }
    str[i] = '\0'; // append string terminator

    // reverse the string
    reverse(str, i);

    return str;
}

char *utoa(u64 num, char *str, i32 base) {
    i32 i = 0;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // process individual digits
    while (num != 0) {
        u64 rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    str[i] = '\0'; // append string terminator

    // reverse the string
    reverse(str, i);

    return str;
}
