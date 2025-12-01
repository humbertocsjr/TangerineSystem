#pragma once 
#include "../kernel.h"
#include "io.h"

void isr_init();
void isr_set(uint8_t isr, void (*action)(regs_t *regs));
