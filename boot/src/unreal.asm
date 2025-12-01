section .text
    unreal_init:
        push ds
        cli
        call nmi_disable
        cld
        mov di, _gdt_table
        mov si, _unreal_gdt_model
        mov cx, _unreal_gdt_size + 1
        rep movsb
        mov word [_gdt_pointer], _unreal_gdt_size
        mov dword [_gdt_pointer+2], _gdt_table
        lgdt [_gdt_pointer]
        mov eax, cr0
        or al, 1
        mov cr0, eax
        jmp 8:.protected_start
    .protected_start:
        mov dx, 0x10
        mov ds, dx
        and al, 0xfe
        mov cr0, eax
        jmp 0:.unreal_start
    .unreal_start:
        pop ds
        call nmi_enable
        sti
        ret


section .data
    _unreal_gdt_model:
        db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0x00, 0x00
        db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00
        .end:
    _unreal_gdt_size: equ _unreal_gdt_model.end - _unreal_gdt_model - 1