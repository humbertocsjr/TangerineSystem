#pragma once

#include "../kernel.h"
#include "../src/io.h"

typedef enum ps2_status_t
{
    PS2_OUT_BUFFER_IS_FULL = 0x01,
    PS2_IN_BUFFER_IS_FULL = 0x02,
    PS2_SYSTEM_FLAG = 0x04,
    PS2_IN_IS_COMMAND = 0x08,
    PS2_TIME_ERROR = 0x40,
    PS2_PARITY_ERROR = 0x80
} ps2_status_t;

void bus_ps2_set_device(uint8_t device, void (*action)(regs_t *regs), void (*init)());

ps2_status_t bus_ps2_read_status();
uint8_t bus_ps2_read_main_u8();
uint8_t bus_ps2_read_aux_u8();
void bus_ps2_write_cmd(uint8_t cmd);
void bus_ps2_write_data_u8(uint8_t value);
bool bus_ps2_can_read_main();
bool bus_ps2_can_read_aux();
void bus_ps2_lock();
void bus_ps2_release();
