#include <stdarg.h>

#include "io.h"
#include "serial.h"
#include "std.h"
#include "types.h"

#define PORT 0x3f8 // COM1

i32 init_serial(void) {
    outb(PORT + 1, 0x00); // Disable all interrupts
    outb(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00); //                  (hi byte)
    outb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outb(PORT + 4, 0x1E); // Set in loopback mode, test the serial chip
    outb(PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check if
                          // serial returns same byte)

    // check if serial is faulty (i.e: not same byte as sent)
    if (inb(PORT + 0) != 0xAE) {
        return 1;
    }

    // if serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(PORT + 4, 0x0F);
    return 0;
}

i32 serial_received(void) { return inb(PORT + 5) & 1; }

i8 read_serial(void) {
    while (serial_received() == 0)
        ;
    return inb(PORT);
}

i32 is_transmit_empty(void) { return inb(PORT + 5) & 0x20; }

void write_serial(char a) {
    while (is_transmit_empty() == 0)
        ;
    if (a == '\n') {
        outb(PORT, '\r');
    }
    outb(PORT, a);
}

i32 com_printf(const char *format, ...) {
    i32 num;
    u64 u_num;
    char s[34];
    char *str;

    va_list args;
    va_start(args, format);

    i32 count = 0;
    while (*format) {
        if (*format != '%') {
            write_serial(*format);
            count++;
        } else {
            switch (*++format) {
            case 'd': // i32
                num = va_arg(args, i32);
                itoa(num, s, 10);
                count += com_printf(s);
                break;
            case 'u': // u32
                u_num = va_arg(args, u32);
                utoa(u_num, s, 10);
                count += com_printf(s);
                break;
            case 'U': // u64
                u_num = va_arg(args, u64);
                utoa(u_num, s, 10);
                count += com_printf(s);
                break;
            case 'c': // char
                num = va_arg(args, i32);
                write_serial(num);
                count++;
                break;
            case 's': // char*
                str = va_arg(args, char *);
                count += com_printf(str);
                break;
            case 'p':
                write_serial('0');
                write_serial('x');
                count += 2;
                /* FALLTHROUGH */
            case 'X':
                u_num = va_arg(args, u64);
                for (i32 i = 16; i > 0; i--) {
                    i32 nibble = (u_num >> (i * 4)) & 0xf;
                    write_serial("0123456789ABCDEF"[nibble]);
                    count++;
                }
                break;
            case 'x':
                num = va_arg(args, i32);
                for (i32 i = 8; i > 0; i--) {
                    i32 nibble = (num >> (i * 4)) & 0xf;
                    write_serial("0123456789ABCDEF"[nibble]);
                    count++;
                }
                break;
            case '0':
                if (*(format + 1) == '2' && *(format + 2) == 'X') {
                    num = va_arg(args, i32);
                    for (i32 i = 2; i > 0; i--) {
                        i32 nibble = (num >> (i * 4)) & 0xf;
                        write_serial("0123456789ABCDEF"[nibble]);
                        count++;
                    }
                    format += 2;
                }
                break;
            case 'f': // f32 (not doing yet)
                break;
            default:
                write_serial('%');
                write_serial(*format);
                count += 2;
                break;
            }
        }
        format++;
    }
    return count;
}
