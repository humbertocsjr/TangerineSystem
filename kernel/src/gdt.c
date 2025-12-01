#include "gdt.h"
#include "io.h"

static gdt_entry_t *_gdt_table = (gdt_entry_t*)HARDWARE_MEM_GDT_TABLE;
static uint32_t _gdt_table_count = 0;

void gdt_set(uint16_t position, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    if(position >= _gdt_table_count) _gdt_table_count = position + 1;
    _gdt_table[position].base_low = (base) & 0xffff;
    _gdt_table[position].base_middle = (base >> 16) & 0xff;
    _gdt_table[position].base_upper = (base >> 24) & 0xff;
    _gdt_table[position].limit_low = (limit) & 0xffff;
    _gdt_table[position].granularity = ((limit >> 16) & 0xf) | ((granularity << 4) & 0xf0);
    _gdt_table[position].access = access;
}

void gdt_init()
{
    gdt_set(0, 0, 0, 0, 0);
    gdt_set(1, 0, 0xffffffff, 0x9a, 0xc);
    gdt_set(2, 0, 0xffffffff, 0x92, 0xc);
    gdt_set(3, 0, 0xffffffff, 0xfa, 0xc);
    gdt_set(4, 0, 0xffffffff, 0xf2, 0xc);
    io_setup_gdt(_gdt_table_count * sizeof(gdt_entry_t));
}