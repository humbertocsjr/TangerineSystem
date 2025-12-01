#include "timer.h"
#include "isr.h"
#include "../config.h"
#include "debug.h"

static volatile void (*_timer_handlers[TIMER_HANDLERS_MAX])(regs_t *regs);
static volatile uint32_t _timer_handlers_next = 0;

volatile uint32_t _ticks = 0;

static void _timer_tick_handler(regs_t *regs)
{
    _ticks++;
    for (uint32_t i = 0; i < _timer_handlers_next; i++)
    {
        _timer_handlers[i](regs);
    }
    
}

void timer_init()
{
    isr_set(0, &_timer_tick_handler);
    uint32_t divisor = 1193180 / 100;
    io_write_u8(0x43, 0x36);
    io_write_u8(0x40, divisor & 0xff);
    io_write_u8(0x40, (divisor >> 8) & 0xff);
}

void timer_add_handler(void (*action)(regs_t *regs))
{
    if(_timer_handlers_next >= TIMER_HANDLERS_MAX)
    {
        debug_print("ERROR: Timer Handlers list overflow.\n");
        return;
    }
    _timer_handlers[_timer_handlers_next] = action;
    _timer_handlers_next++;
}

void timer_delay_ticks(uint32_t ticks)
{
    uint32_t start = _ticks;
    while((start + ticks) < _ticks)
    {
        __asm__("hlt");
    }
}

void timer_delay_ms(uint32_t ms)
{
    timer_delay_ticks(ms / 10);
}