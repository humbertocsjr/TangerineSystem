cpu 386
bits 32
section .text
    dd _start
    dd args
    extern _etext
    dd _etext
    extern _edata
    dd _edata
    extern _end
    dd _end

section .bss
    align 16
    stack_botton:
        resb 32768
    stack_top:
    args:
        resb 256

section .text
    global _start
    _start:
        mov esp, stack_top
        extern kernel_init
        call kernel_init
        cli
        .loop:
            hlt
            jmp .loop