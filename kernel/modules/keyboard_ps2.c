#include "../kernel.h"
#include "bus_ps2.h"
#include "../src/keyboard.h"
#include "../src/debug.h"
#include "../src/timer.h"
#include <stdint.h>

static bool _ignore = true;
static bool _shift = false;


static char _map_normal_pressed[256] =
{
//    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0000,0300, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',0010,0011, // 0x0X
     'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',0021,0301, 'a', 's', // 0x1X
     'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',0047, '`',0303,0134, 'z', 'x', 'c', 'v', // 0x2X
     'b', 'n', 'm', ',', '.', '/',0304,0326,0305, ' ',0300,0311,0312,0313,0314,0315, // 0x3X
//    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0316,0317,0320,0321,0322,0307,0310,0327,0330,0331,0332,0333,0334,0335,0336,0337, // 0x4X
    0340,0341,0342,0343,0000,0000,0000,0323,0324,0000,0000,0000,0000,0000,0000,0000, // 0x5X
    0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000, // 0x6X
    0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000, // 0x7X
};


static void _action(regs_t *regs)
{
    if(_ignore) return;
    uint8_t scancode = bus_ps2_read_main_u8();
    uint8_t pressed = 0;
    uint8_t released = 0;
    char *map = _map_normal_pressed;
    if(scancode == 0xe0)
    {

    }
    else if(scancode == 0xe1)
    {

    }
    else if(scancode & 0x80)
    {
        released = map[scancode & 0x7f];
    }
    else
    {
        pressed = map[scancode & 0x7f];
    }
    switch(pressed)
    {
        case KEY_LEFT_SHIFT: case KEY_RIGHT_SHIFT: _shift = true; break;
    }
    switch(released)
    {
        case KEY_LEFT_SHIFT: case KEY_RIGHT_SHIFT: _shift = false; break;
    }

    if(pressed) debug_print("KEY PRESS   %x '%c' [%s]\n", pressed, (pressed >= 0300 | pressed < ' ') ? 0 : pressed, _shift ? "SHIFT" : "");
    if(released) debug_print("KEY RELEASE %x '%c' [%s]\n", released, (released >= 0300 | released < ' ') ? 0 : released, _shift ? "SHIFT" : "");
}

static void _set_leds(bool scroll_lock, bool num_lock, bool caps_lock)
{
    bus_ps2_lock();
    bus_ps2_write_cmd(0xae);
    bus_ps2_write_data_u8((scroll_lock ? 1 : 0) | (num_lock ? 2: 0) | (caps_lock ? 4 : 0));
    bus_ps2_release();
}

static bool _try_read(uint8_t *value)
{
    uint32_t timeout = 200;
    while(!bus_ps2_can_read_main() && timeout--) timer_delay_ticks(1);
    if(bus_ps2_can_read_main())
    {
        *value = bus_ps2_read_main_u8();
        return true;
    }
    return false;
}

static void _setup()
{
    uint8_t id[2];
    uint32_t timeout;
    debug_print("Initializing PS/2 keyboard");
    _set_leds(false, false, false);
    bus_ps2_lock();
    // limpa buffer
    while(bus_ps2_can_read_main()) bus_ps2_read_main_u8();
    // define scancode 1
    bus_ps2_write_data_u8(0xf0);
    bus_ps2_write_data_u8(0x01);
    // limpa buffer
    while(bus_ps2_can_read_main()) bus_ps2_read_main_u8();
    // ativa leitura de teclas
    bus_ps2_write_data_u8(0xf4);
    timer_delay_ms(10);
    bus_ps2_read_main_u8();
    timer_delay_ms(10);
    while(bus_ps2_can_read_main())
    {
        bus_ps2_read_main_u8();
        timer_delay_ms(10);
    }
    bus_ps2_release();
    _ignore = false;
    debug_print(".ok\n");
}

MODULE(keyboard_ps2)
{
    bus_ps2_set_device(0, &_action, &_setup);
    return true;
}
