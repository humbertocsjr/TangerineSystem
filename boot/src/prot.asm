section .text
    prot_init:
        call debug_print_inline
        db "Entering protected mode",0
        cli
        call nmi_disable
        cld
        mov di, _gdt_table
        mov si, _prot_gdt_model
        mov cx, _prot_gdt_size + 1
        rep movsb
        mov word [_gdt_pointer], _prot_gdt_size
        mov dword [_gdt_pointer+2], _gdt_table
        lgdt [_gdt_pointer]
        mov eax, cr0
        or al, 1
        mov cr0, eax
        jmp 8:.protected_start
    .protected_start:
        bits 32
        mov dx, 0x10
        mov ds, dx
        mov es, dx
        mov fs, dx
        mov gs, dx
        mov ss, dx
        mov esp, 0x7c00
        call debug32_print_inline
        db ".ok",13,10,0
        jmp start32
        bits 16

section .data
    _prot_gdt_model:
        db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0xfa, 0xcf, 0x00
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0xf2, 0xcf, 0x00
        .end:
    _prot_gdt_size: equ _prot_gdt_model.end - _prot_gdt_model - 1