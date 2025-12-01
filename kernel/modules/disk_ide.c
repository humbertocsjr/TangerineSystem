#include "../kernel.h"
#include "../src/io.h"
#include "../src/debug.h"
#include <stdint.h>


static void _write_ctl(int channel, int port, uint8_t value)
{
    io_write_u8((channel < 2 ? 0x3f4 : 0x374) + port, value);
}

static void _write_data(int channel, int port, uint16_t value)
{
    io_write_u16((channel < 2 ? 0x1f0 : 0x170) + port, value);
}

static uint16_t _read_data(int channel, int port)
{
    return io_read_u16((channel < 2 ? 0x1f0 : 0x170) + port);
}

MODULE(disk_ide)
{
    debug_print("Initializing IDE Controller");

    debug_print(".ok\n");
    return true;
}
