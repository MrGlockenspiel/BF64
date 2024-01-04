#include <stdarg.h>

#include "fb.h"
#include "fbcon.h"
#include "font.h"
#include "std.h"

static i16 cur_x = 0;
static i16 cur_y = 16;
static u32 cur_color = 0xFFFFFFFF;

i32 fb_printf(const char *format, ...) {
    i32 num;
    char s[34];
    char *str;

    va_list args;
    va_start(args, format);

    i32 count = 0;
    while (*format) {
        if (*format != '%') {
            if (*format != '\n') {
                fb_drawchar(cur_x, cur_y, *format, cur_color);
                cur_x += 8;
            } else {
                cur_y += 16;
                cur_x = 0;
            }
            count++;
        } else {
            switch (*++format) {
            case 'd': // i32
                num = va_arg(args, i32);
                itoa(num, s, 10);
                count += fb_printf(s);
                break;
            case 'c': // char
                num = va_arg(args, i32);
                fb_drawchar(cur_x, cur_y, num, cur_color);
                cur_x += 8;
                count++;
                break;
            case 's': // char*
                str = va_arg(args, char *);
                count += fb_printf(str);
                break;
            case 'p':
                count += 2;
                fb_putchar('0');
                fb_putchar('x');
                /* FALLTHROUGH */
            case 'x':
            case 'X':
                num = va_arg(args, i32);
                for (i32 i = 8; i > 0; i--) {
                    i32 nibble = (num >> (i * 4)) & 0xf;
                    fb_drawchar(cur_x, cur_y, "0123456789ABCDEF"[nibble],
                                cur_color);
                    cur_x += 8;
                    count++;
                }
                break;
            case '0':
                if (*(format + 1) == '2' && *(format + 2) == 'X') {
                    num = va_arg(args, i32);
                    for (i32 i = 2; i > 0; i--) {
                        i32 nibble = (num >> (i * 4)) & 0xf;
                        fb_drawchar(cur_x, cur_y, "0123456789ABCDEF"[nibble],
                                    cur_color);
                        cur_x += 8;
                        count++;
                    }
                    format += 2;
                }
                break;
            case 'f': // f32 (not doing)
                break;
            default:
                fb_drawchar(cur_x, cur_y, '%', cur_color);
                cur_x += 8;
                fb_drawchar(cur_x, cur_y, *format, cur_color);
                cur_x += 8;
                count += 2;
                break;
            }
        }
        format++;
    }
    return count;
}

void fb_putchar(char c) {
    if (c == '\n') {
        cur_x = 0;
        cur_y += 16;
    } else {
        fb_drawchar(cur_x, cur_y, c, cur_color);
        cur_x += 8;
    }

    if ((u32)cur_y > fb_getheight()) {
        fb_setbg(0x00000000);
        cur_x = 0;
        cur_y = 16;
    }
}
