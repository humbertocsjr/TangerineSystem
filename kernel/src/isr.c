#include "isr.h"
#include "handler.h"

void isr_init()
{
    io_write_u8(0x20, 0x11);
    io_write_u8(0xa0, 0x11);
    io_write_u8(0x21, 0x20);
    io_write_u8(0xa1, 0x28);
    io_write_u8(0x21, 0x4);
    io_write_u8(0xa1, 0x2);
    io_write_u8(0x21, 0x1);
    io_write_u8(0xa1, 0x1);
    io_write_u8(0x21, 0x0);
    io_write_u8(0xa1, 0x0);
}

void isr_set(uint8_t isr, void (*action)(regs_t *regs))
{
    handler_set((isr & 0xf) + 32, action);
}