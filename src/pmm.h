#pragma once

#include "types.h"

#define TO_KIB(x) ((x) / 1024)
#define TO_MIB(x) (TO_KIB(x) / 1024)
#define TO_GIB(x) (TO_MIB(x) / 1024)

typedef struct chunk_s {
    u64 length;
    u8 *base;
    u8 *current_base;
} chunk_t;

i32 init_pmm(void);
void *pmalloc(u64 s);
