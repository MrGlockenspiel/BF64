#pragma once

#include "types.h"

i32 init_serial(void);
i32 serial_received(void);
i8 read_serial(void);
i32 is_transmit_empty(void);
void write_serial(char a);
i32 com_printf(const char *format, ...);
