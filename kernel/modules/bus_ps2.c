#include "../kernel.h"
#include "../src/io.h"
#include "../src/isr.h"
#include "bus_ps2.h"
#include "../src/debug.h"
#include "../src/timer.h"

static bool _received_main = false;
static uint8_t _value_main = 0;
static bool _received_aux = false;
static uint8_t _value_aux = 0;
static bool _initialized = false;
static void (*_main_init)() = 0;
static void (*_aux_init)() = 0;
static bool _lock = false;

static void (*_bus_ps2_devices[2])(regs_t *reg);

static void _action_dev_main(regs_t *regs)
{
    _received_main = true;
    _value_main = io_read_u8(0x60);
    if(_bus_ps2_devices[0] && _initialized)
    {
        _bus_ps2_devices[0](regs);
    }
}

static void _action_dev_aux(regs_t *regs)
{
    _received_aux = true;
    _value_aux = io_read_u8(0x60);
    if(_bus_ps2_devices[1] && _initialized) _bus_ps2_devices[1](regs);
}

MODULE(bus_ps2)
{
    debug_print("Initializing PS/2 Bus");
    _bus_ps2_devices[0] = 0;
    _bus_ps2_devices[1] = 0;
    // atribui interrupcoes
    isr_set(1, _action_dev_main);
    isr_set(12, _action_dev_aux);
    debug_print(".");
    // ativa interrupcoes
    bus_ps2_write_cmd(0x60);
    bus_ps2_write_data_u8(0x01|0x02);
    debug_print(".");
    // desativa dispositivos nas duas portas
    bus_ps2_write_cmd(0xad);
    bus_ps2_write_cmd(0xa7);
    debug_print(".");
    // limpa buffer
    while(bus_ps2_read_status() & PS2_OUT_BUFFER_IS_FULL) io_read_u8(0x60);
    debug_print(".");
    // ativa dispositivos
    bus_ps2_write_cmd(0xae);
    bus_ps2_write_cmd(0xa8);
    debug_print(".");
    // le byte de configuracao
    bus_ps2_write_cmd(0x20);
    while(bus_ps2_read_status() & PS2_OUT_BUFFER_IS_FULL) io_read_u8(0x60);
    debug_print(".");
    // definie como inicializado
    _initialized = true;
    debug_print(".ok\n");
    // inicializa dispositivos registrados
    if(_main_init) _main_init();
    if(_aux_init) _aux_init();
    return true;
}


void bus_ps2_set_device(uint8_t device, void (*action)(regs_t *regs), void (*init)())
{
    if(device > 2) return;
    IO_STORE_FLAGS();
    io_disable_int();
    _bus_ps2_devices[device] = action;
    if(device == 0) _main_init = init;
    if(device == 1) _aux_init = init;
    IO_RESTORE_FLAGS();
    if(_initialized) init();
}

ps2_status_t bus_ps2_read_status()
{
    return io_read_u8(0x64);
}

uint8_t bus_ps2_read_main_u8()
{
    uint32_t timeout_counter = 100;
    while(timeout_counter-- && !_received_main)
    {
        timer_delay_ticks(1);
    }
    _received_main = false;
    return _value_main;
}

uint8_t bus_ps2_read_aux_u8()
{
    uint32_t timeout_counter = 100;
    while(timeout_counter-- && !_received_aux)
    {
        timer_delay_ticks(1);
    }
    _received_aux = false;
    return _received_aux;
}

bool bus_ps2_can_read_main()
{
    return _received_main;
}

bool bus_ps2_can_read_aux()
{
    return _received_aux;
}

void bus_ps2_write_cmd(uint8_t cmd)
{
    while(true) if(!(bus_ps2_read_status() & PS2_IN_BUFFER_IS_FULL)) break;
    io_write_u8(0x64, cmd);
}

void bus_ps2_write_data_u8(uint8_t value)
{
    while(true) if(!(bus_ps2_read_status() & PS2_IN_BUFFER_IS_FULL)) break;
    io_write_u8(0x60, value);
}

void bus_ps2_lock()
{
    while(true)
    {
        while(_lock)
        {
            timer_delay_ticks(1);
        }
        IO_STORE_FLAGS();
        io_disable_int();
        if(!_lock)
        {
            _lock = true;
            IO_RESTORE_FLAGS();
            return;
        }
        IO_RESTORE_FLAGS();
    }
}

void bus_ps2_release()
{
    _lock = false;
}
