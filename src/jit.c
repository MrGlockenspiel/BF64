#include <stdarg.h>

#include "fbcon.h"
#include "jit.h"
#include "serial.h"
#include "std.h"

void push_ins(u8 *memory, u64 *index, i32 count, ...) {
    va_list args;
    va_start(args, count);

    for (i32 i = 0; i < count; i++) {
        u8 ins = (u8)va_arg(args, i32);
        memory[(*index)++] = ins;
    }
    va_end(args);
    return;
}

void push_u32(u8 *memory, u64 *index, u32 value) {
    push_ins(memory, index, 4, value & 0xFF, (value >> 8) & 0xFF,
             (value >> 16) & 0xFF, (value >> 24) & 0xFF);

    return;
}

void push_u64(u8 *memory, u64 *index, u64 value) {
    push_ins(memory, index, 8, (u8)(value & 0xFF), (u8)((value >> 8) & 0xFF),
             (u8)((value >> 16) & 0xFF), (u8)((value >> 24) & 0xFF),
             (u8)((value >> 32) & 0xFF), (u8)((value >> 40) & 0xFF),
             (u8)((value >> 48) & 0xFF), (u8)((value >> 56) & 0xFF));

    return;
}

void push_addr(u8 *memory, u64 *index, void *address) {
    u64 value = (u64)address;
    push_ins(memory, index, 8, (u8)(value & 0xFF), (u8)((value >> 8) & 0xFF),
             (u8)((value >> 16) & 0xFF), (u8)((value >> 24) & 0xFF),
             (u8)((value >> 32) & 0xFF), (u8)((value >> 40) & 0xFF),
             (u8)((value >> 48) & 0xFF), (u8)((value >> 56) & 0xFF));

    return;
}

void asm_add_r13(u8 *memory, u64 *index, u32 val) {
    // add r13, val
    push_ins(memory, index, 3, 0x49, 0x81, 0xC5);
    push_u32(memory, index, val);
    return;
}

void asm_sub_r13(u8 *memory, u64 *index, u32 val) {
    // sub r13, val
    push_ins(memory, index, 3, 0x49, 0x81, 0xED);
    push_u32(memory, index, val);
    return;
}

void asm_inc_r13(u8 *memory, u64 *index) {
    // inc r13
    push_ins(memory, index, 3, 0x49, 0xFF, 0xC5);
    return;
}

void asm_dec_r13(u8 *memory, u64 *index) {
    // dec r13
    push_ins(memory, index, 3, 0x49, 0xFF, 0xCD);
    return;
}

void asm_add_ptr_r13(u8 *memory, u64 *index, u8 val) {
    // add byte [r13], val
    push_ins(memory, index, 5, 0x41, 0x80, 0x45, 0x00, val);
    return;
}

void asm_sub_ptr_r13(u8 *memory, u64 *index, u8 val) {
    // sub byte [r13], val
    push_ins(memory, index, 5, 0x41, 0x80, 0x6D, 0x00, val);
    return;
}

void asm_mov_r13_qword(u8 *memory, u64 *index, u64 val) {
    // mov r13, qword val
    push_ins(memory, index, 2, 0x49, 0xBD);
    push_u64(memory, index, val);
    return;
}

void asm_mov_r13_arg1(u8 *memory, u64 *index) {
    push_ins(memory, index, 3, 0x49, 0x89, 0xFD);
    return;
}

void asm_putchar(u8 *memory, u64 *index) {
    // mov rax, qword function
    push_ins(memory, index, 2, 0x48, 0xB8);
    push_u64(memory, index, (u64)fb_putchar);

    // mov dil, byte [r13]
    push_ins(memory, index, 4, 0x41, 0x8A, 0x7D, 0x00);

    // call rax
    push_ins(memory, index, 2, 0xFF, 0xD0);
    return;
}

void asm_getchar(u8 *memory, u64 *index) {
    // mov rax qword function
    push_ins(memory, index, 2, 0x48, 0xB8);
    push_u64(memory, index, (u64)read_serial);

    // call rax
    push_ins(memory, index, 2, 0xFF, 0xD0);

    // mov [r13], rax
    push_ins(memory, index, 4, 0x49, 0x89, 0x45, 0x00);
    return;
}

void asm_cmp_byte_r13(u8 *memory, u64 *index, u8 value) {
    // cmp byte [r13], 0
    push_ins(memory, index, 4, 0x41, 0x80, 0x7D, 0x00);
    push_ins(memory, index, 1, value);
    return;
}

void asm_jz(u8 *memory, u64 *index, u32 address) {
    // jz near address
    push_ins(memory, index, 2, 0x0F, 0x84);
    push_u32(memory, index, address);
    return;
}

void asm_jnz(u8 *memory, u64 *index, u32 address) {
    // jnz near address
    push_ins(memory, index, 2, 0x0F, 0x85);
    push_u32(memory, index, address);
    return;
}

void asm_ret(u8 *memory, u64 *index) {
    // ret
    push_ins(memory, index, 1, 0xC3);
    return;
}
