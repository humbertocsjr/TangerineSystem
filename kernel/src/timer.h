#pragma once
#include "io.h"
#include "../kernel.h"

void timer_init();
void timer_add_handler(void (*action)(regs_t *regs));
void timer_delay_ticks(uint32_t ticks);
void timer_delay_ms(uint32_t ms);