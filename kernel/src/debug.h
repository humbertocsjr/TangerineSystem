#pragma once

#include <stdint.h>
#include "../../config.h"

#ifdef DEBUG

    void debug_print_char(char c);
    void debug_print_string(char *str);
    void debug_print_u32(uint32_t value);
    void debug_print_s32(int32_t value);
    void debug_print(char *fmt, ...);
    #define debug_breakpoint() __asm__ __volatile__("xchg %bx, %bx");

#else

    #define debug_print_char(value) ;
    #define debug_print_string(value) ;
    #define debug_print_u32(value) ;
    #define debug_print_s32(value) ;
    #define debug_print(__VA_ARGS__) ;
    #define debug_breakpoint() ;

#endif