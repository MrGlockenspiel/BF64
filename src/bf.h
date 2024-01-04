#pragma once

#include "jit.h"
#include "types.h"

program_t *bf_parse_string(const char *str);
void bf_interpret(const program_t *program);
void bf_compile(const program_t *program);
void dump_cells(u8 *cell_memory, u64 n);
