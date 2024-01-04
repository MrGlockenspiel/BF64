#pragma once

#include "types.h"

void fb_drawchar(u32 x, u32 y, char c, u32 color);
void fb_drawstr(u32 x, u32 y, const char *str, u32 color);
