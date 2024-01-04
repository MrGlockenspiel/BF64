#pragma once

#include "types.h"

typedef struct opcode_t {
    char op;
    union {
        u64 count;
        u64 jmp_addr;
    };
} opcode_t;

typedef struct program_t {
    u64 size;
    opcode_t *bytecode;
} program_t;

void push_ins(u8 *memory, u64 *index, i32 count, ...);
void push_u32(u8 *memory, u64 *index, u32 value);
void push_u64(u8 *memory, u64 *index, u64 value);
void push_addr(u8 *memory, u64 *index, void *address);

void asm_add_r13(u8 *memory, u64 *index, u32 val);
void asm_sub_r13(u8 *memory, u64 *index, u32 val);
void asm_inc_r13(u8 *memory, u64 *index);
void asm_dec_r13(u8 *memory, u64 *index);

void asm_add_ptr_r13(u8 *memory, u64 *index, u8 val);
void asm_sub_ptr_r13(u8 *memory, u64 *index, u8 val);

void asm_mov_r13_qword(u8 *memory, u64 *index, u64 val);
void asm_mov_r13_arg1(u8 *memory, u64 *index);

void asm_putchar(u8 *memory, u64 *index);
void asm_getchar(u8 *memory, u64 *index);

void asm_cmp_byte_r13(u8 *memory, u64 *index, u8 value);

void asm_jz(u8 *memory, u64 *index, u32 address);
void asm_jnz(u8 *memory, u64 *index, u32 address);

void asm_ret(u8 *memory, u64 *index);
