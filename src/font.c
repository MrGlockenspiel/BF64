#include <stddef.h>

#include "fb.h"
#include "ibm_vga_8x16.h"
#include "types.h"

#define FONT ibm_vga_8x16
#define WIDTH 8
#define HEIGHT 16

void fb_drawchar(u32 x, u32 y, char c, u32 color) {
    u16 cx, cy;
    const u8 mask[8] = {0b10000000, 0b01000000, 0b00100000, 0b00010000,
                        0b00001000, 0b00000100, 0b00000010, 0b00000001};

    const u8 *glyph = FONT + (i32)c * 16;

    for (cy = 0; cy < HEIGHT; cy++) {
        for (cx = 0; cx < WIDTH; cx++) {
            if (glyph[cy] & mask[cx]) {
                fb_drawpixel(x + cx, y + cy - 12, color);
            }
        }
    }
}

void fb_drawstr(u32 x, u32 y, char *str, u32 color) {
    u16 cx = x;
    u16 cy = y;
    while (*str) {
        if (*str == '\n') {
            cy += HEIGHT;
            cx = x;
        } else {
            fb_drawchar(cx, cy, *str, color);
            cx += WIDTH;
        }
        str++;
    }
}

#undef FONT
#undef WIDTH
#undef HEIGHT
