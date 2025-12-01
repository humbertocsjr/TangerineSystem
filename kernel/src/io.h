#pragma once
#include <stdint.h>

#define IO_STORE_FLAGS() __asm__("pushf");
#define IO_RESTORE_FLAGS() __asm__("popf");

typedef struct regs_t
{
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t unused;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} regs_t;

void io_write_u8(uint16_t port, uint8_t value);
void io_write_u16(uint16_t port, uint8_t value);
void io_write_u32(uint16_t port, uint8_t value);
uint8_t io_read_u8(uint16_t port);
uint16_t io_read_u16(uint16_t port);
uint32_t io_read_u32(uint16_t port);
void io_setup_gdt(uint16_t size);
void io_setup_idt(uint16_t size);
void io_enable_int();
void io_disable_int();
void io_enable_nmi();
void io_disable_nmi();
