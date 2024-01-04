#include "types.h"

u8 inb(u16 port) {
    u8 data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

void outb(u16 port, u8 data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}
