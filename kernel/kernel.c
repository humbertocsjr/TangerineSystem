#include "kernel.h"
#include "src/debug.h"
#include "src/video.h"
#include "src/gdt.h"
#include "src/idt.h"
#include "src/io.h"
#include "src/handler.h"
#include "src/isr.h"
#include "src/timer.h"
#include "src/vfs.h"

extern char args[];

void modules_init();

void kernel_init()
{
    module_t *mod;
    debug_print_string("Tangerine System Software\n");
    modules_init();
    handler_init();
    gdt_init();
    idt_init();
    isr_init();
    timer_init();
    video_init();
    vfs_init();
    debug_print_string("done\n");
    io_enable_int();
    io_enable_nmi();

    mod = modules_find("bus_ps2");
    if(mod) mod->init();
    mod = modules_find("keyboard_ps2");
    if(mod) mod->init();
    mod = modules_find("disk_ide");
    if(mod) mod->init();

    while(true) __asm("hlt");
}
