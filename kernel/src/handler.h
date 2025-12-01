#pragma once
#include "../kernel.h"
#include "io.h"

void handler_init();

void handler_set(uint8_t interrupt, void (*action)(regs_t *regs));