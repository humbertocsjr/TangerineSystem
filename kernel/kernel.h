#pragma once

#include <stdint.h>
#include <stdbool.h>

#define HARDWARE_MEM_GDT_POINTER 0x6000
#define HARDWARE_MEM_GDT_TABLE 0x6010
#define HARDWARE_MEM_IDT_POINTER 0x7000
#define HARDWARE_MEM_IDT_TABLE 0x7010

#define MODULE(module) __asm__(".align 128\n.long 0x12344321\n.long 0x43211234\n.long " #module "_init\n.asciz \"" #module "\"\n"); bool module##_init()

typedef struct module_t
{
    uint32_t signature1 __attribute__((__packed__));
    uint32_t signature2 __attribute__((__packed__));
    bool (*init)() __attribute__((__packed__));
    char name[116] __attribute__((__packed__));
} module_t;

module_t *modules_find(char *name);
