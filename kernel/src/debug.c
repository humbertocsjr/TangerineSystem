#include "io.h"
#include <stdarg.h>
#include <stdbool.h>


void debug_print_char(char c)
{
    io_write_u8(0xe9, c);
}

void debug_print_string(char *str)
{
    while (*str)
    {
        debug_print_char(*str++);
    }
}

void debug_print_u32(uint32_t value)
{
    if ((value / 10))
        debug_print_u32(value / 10);
    debug_print_char((value % 10) + '0');
}

void debug_print_x32(uint32_t value, bool upper)
{
    char *hex = upper ? "012345678ABCDEF" : "0123456789abcdef";
    debug_print_char(hex[(value >> 28) & 0xf]);
    debug_print_char(hex[(value >> 24) & 0xf]);
    debug_print_char(hex[(value >> 20) & 0xf]);
    debug_print_char(hex[(value >> 16) & 0xf]);
    debug_print_char(hex[(value >> 12) & 0xf]);
    debug_print_char(hex[(value >> 8) & 0xf]);
    debug_print_char(hex[(value >> 4) & 0xf]);
    debug_print_char(hex[(value) & 0xf]);
}

void debug_print_s32(int32_t value)
{
    if (value < 0)
    {
        debug_print_char('-');
        value = -value;
    }
    debug_print_u32(value);
}

void debug_print(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    while (*fmt)
    {
        switch (*fmt)
        {
            case '%':
                fmt++;
                switch(*fmt)
                {
                    case 's':
                        debug_print_string(va_arg(args, char *));
                        break;
                    case 'u':
                        debug_print_u32(va_arg(args, uint32_t));
                        break;
                    case 'd':
                        debug_print_s32(va_arg(args, int32_t));
                        break;
                    case 'x':
                        debug_print_x32(va_arg(args, int32_t), false);
                        break;
                    case 'X':
                        debug_print_x32(va_arg(args, int32_t), true);
                        break;
                    case 'c':
                        debug_print_char(va_arg(args, char));
                        break;
                    default:
                        debug_print_char(*fmt);
                        break;
                }
                break;

            default:
                debug_print_char(*fmt);
                break;
        }
        if(*fmt) fmt++;
    }
    va_end(args);
}
