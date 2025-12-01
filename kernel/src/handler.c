#include "handler.h"
#include <string.h>
#include "debug.h"

void (*_handlers[256])(regs_t *regs);

void handler_init()
{
    memset(_handlers, 0, sizeof(_handlers));
}

void handler_set(uint8_t interrupt, void (*action)(regs_t *regs))
{
    _handlers[interrupt] = action;
}

void _idt_handler(regs_t regs)
{
    if(_handlers[regs.interrupt & 0xff])
    {
        _handlers[regs.interrupt & 0xff](&regs);
    }
    else if(regs.interrupt < 32)
    {
        debug_print("<INT 0x%x (CPU) NOT HANDLED BY KERNEL>\n", regs.interrupt);
    }
    else if(regs.interrupt < 48)
    {
        debug_print("<INT 0x%x (ISR %u) NOT HANDLED BY KERNEL>\n", regs.interrupt, regs.interrupt - 32);
    }
    else
    {
        debug_print("<INT 0x%x (UNKNOWN) NOT HANDLED BY KERNEL>\n", regs.interrupt);
    }
    if(regs.interrupt >= 32 && regs.interrupt < 48)
    {
        if(regs.interrupt >= 40) io_write_u8(0xa0, 0x20);
        io_write_u8(0x20, 0x20);
    }

}