#include <stddef.h>

#include "limine.h"
#include "serial.h"
#include "std.h"
#include "types.h"

/* framebuffer structures
struct limine_framebuffer {
  void *address;
  uint64_t width;
  uint64_t height;
  uint64_t pitch;
  uint16_t bpp; // Bits per pixel
  uint8_t memory_model;
  uint8_t red_mask_size;
  uint8_t red_mask_shift;
  uint8_t green_mask_size;
  uint8_t green_mask_shift;
  uint8_t blue_mask_size;
  uint8_t blue_mask_shift;
  uint8_t unused[7];
  uint64_t edid_size;
  void *edid;

  // Response revision 1
  uint64_t mode_count;
  struct limine_video_mode **modes;
};

struct limine_video_mode {
  uint64_t pitch;
  uint64_t width;
  uint64_t height;
  uint16_t bpp;
  uint8_t memory_model;
  uint8_t red_mask_size;
  uint8_t red_mask_shift;
  uint8_t green_mask_size;
  uint8_t green_mask_shift;
  uint8_t blue_mask_size;
  uint8_t blue_mask_shift;
};
*/

// request fb from limine
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = NULL,
};

// global fb
static struct limine_framebuffer *fb;

i32 init_fb(void) {
    // ensure we got a framebuffer
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {
        return 1;
    }

    // fetch the first framebuffer.
    fb = framebuffer_request.response->framebuffers[0];

    com_printf("fb->width: %d\r\n", fb->width);
    com_printf("fb->height: %d\r\n", fb->height);
    com_printf("fb->address: 0x%p\r\n", fb->address);
    com_printf("fb->pitch: %d\r\n", fb->pitch);
    com_printf("fb->bpp: %d\r\n", fb->bpp);
    com_printf("fb->memory_model: %d\r\n", fb->memory_model);

    return 0;
}

void fb_drawpixel(u32 x, u32 y, u32 color) {
    if (x > fb->width)
        return;
    if (y > fb->height)
        return;

    u32 where = x * sizeof(u32) + y * fb->pitch;

    ((u8 *)fb->address)[where] = color & 255;             // BLUE
    ((u8 *)fb->address)[where + 1] = (color >> 8) & 255;  // GREEN
    ((u8 *)fb->address)[where + 2] = (color >> 16) & 255; // RED
    //((u32*)fb->address)[where] = color;
}

void fb_drawline(i32 x0, i32 y0, i32 x1, i32 y1, u32 color) {
    i32 dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    i32 dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    i32 err = dx + dy, e2; /* error value e_xy */

    while (1) { /* loop */
        fb_drawpixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        } /* e_xy+e_x > 0 */
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        } /* e_xy+e_y < 0 */
    }
}

void fb_setbg(u32 color) {
    for (u32 i = 0; i < fb->width * fb->height; i++) {
        ((u32 *)fb->address)[i] = color;
    }
}

u32 fb_getwidth(void) { return fb->width; }

u32 fb_getheight(void) { return fb->height; }
