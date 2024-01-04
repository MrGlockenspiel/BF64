#include <stddef.h>
#include <string.h>

#include "limine.h"
#include "pmm.h"
#include "serial.h"
#include "types.h"

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = NULL,
};

u64 free_memory = 0;

u64 alloc_chunk_size = 0;
void *alloc_chuck_start = NULL;
void *current_alloc_ptr = NULL;
void *alloc_chunk_end = NULL;

static const char *memmap_type[] = {
    "USABLE",
    "RESERVED",
    "ACPI RECLAIMABLE",
    "ACPI NVS",
    "BAD MEMORY",
    "BOOTLOADER RECLAIMABLE",
    "KERNEL AND MODULES",
    "FRAMEBUFFER",
};

i32 init_pmm(void) {
    if (memmap_request.response == NULL) {
        return 1;
    }

    com_printf("Memory map: \n");

    struct limine_memmap_entry *current_entry;

    for (u64 i = 0; i < memmap_request.response->entry_count; i++) {
        current_entry = memmap_request.response->entries[i];

        com_printf("\tEntry #%d: \n\t\tBase: 0x%X\n\t\tLength: %U B (%U "
                   "KiB)\n\t\tType: %s\n",
                   i, current_entry->base, current_entry->length,
                   TO_KIB(current_entry->length),
                   memmap_type[current_entry->type]);

        if (current_entry->type != LIMINE_MEMMAP_USABLE &&
            current_entry->type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE &&
            current_entry->type != LIMINE_MEMMAP_KERNEL_AND_MODULES) {
            continue;
        }

        if ((u64)current_entry->length > (u64)alloc_chunk_size) {
            alloc_chunk_size = (u64)current_entry->length;
            alloc_chuck_start = (void *)current_entry->base;
            current_alloc_ptr = (void *)current_entry->base;
            alloc_chunk_end =
                (void *)((u64)current_entry->length + (u64)current_entry->base);
        }

        free_memory += current_entry->length;
    }

    com_printf("Free Memory: %U B (%U KiB) (%U MiB)\n", free_memory,
               TO_KIB(free_memory), TO_MIB(free_memory));
    com_printf("Largest chunk: %U B (%U KiB) (%U MiB) at addr %p\n",
               alloc_chunk_size, TO_KIB(alloc_chunk_size),
               TO_MIB(alloc_chunk_size), alloc_chuck_start);

    return 0;
}

void *pmalloc(u64 s) {
    void *new_base = (void *)((u64)current_alloc_ptr + (u64)s);

    if ((u64)new_base > (u64)alloc_chunk_end) {
        com_printf("Failed to alloc %U bytes\n", s);
        return NULL;
    }

    void *addr = current_alloc_ptr;
    current_alloc_ptr = new_base;
    return (void *)addr;
}
