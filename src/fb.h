#pragma once

#include "types.h"

i32 init_fb(void);
void fb_setbg(u32 color);
void fb_drawpixel(u32 x, u32 y, u32 color);
void fb_drawline(i32 x0, i32 y0, i32 x1, i32 y1, u32 color);
u32 fb_getwidth(void);
u32 fb_getheight(void);
