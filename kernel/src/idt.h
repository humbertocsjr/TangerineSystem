#pragma once

#include "../kernel.h"

extern void _idt_global_handler();

#define IDT_WITH_ERR(num) __asm__(".globl _idt_" #num "\n.align 4\n_idt_" #num ":\nnop\npushl $" #num "\njmp _idt_global_handler\n");
#define IDT_WOUT_ERR(num) __asm__(".globl _idt_" #num "\n.align 4\n_idt_" #num ":\nnop\npushl $0\npushl $" #num "\njmp _idt_global_handler\n");

#define IDT_HANDLER(num) extern void _idt_##num(); _idt_setup(num, (uint32_t)&_idt_##num);

typedef struct idt_entry_t
{
    uint16_t base_lower __attribute__((__packed__));
    uint16_t selector __attribute__((__packed__));
    uint8_t zero __attribute__((__packed__));
    uint8_t flags __attribute__((__packed__));
    uint16_t base_upper __attribute__((__packed__));
} idt_entry_t;

extern void idt_init();