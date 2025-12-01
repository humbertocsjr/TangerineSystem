#pragma once

#include "../kernel.h"
#include <stdint.h>

typedef struct gdt_entry_t
{
    uint16_t limit_low __attribute__((packed));
    uint16_t base_low __attribute__((packed));
    uint8_t base_middle __attribute__((packed));
    uint8_t access __attribute__((packed));
    uint8_t granularity __attribute__((packed));
    uint8_t base_upper __attribute__((packed));
} gdt_entry_t;


void gdt_init();