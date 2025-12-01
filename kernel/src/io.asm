
cpu 386
bits 32
section .text

    global io_write_u8
    io_write_u8:
        mov edx, [esp + 4]
        mov eax, [esp + 8]
        out dx, al
        xor eax, eax
        ret

    global io_write_u16
    io_write_u16:
        mov edx, [esp + 4]
        mov eax, [esp + 8]
        out dx, ax
        xor eax, eax
        ret

    global io_write_u32
    io_write_u32:
        mov edx, [esp + 4]
        mov eax, [esp + 8]
        out dx, eax
        xor eax, eax
        ret

    global io_read_u8
    io_read_u8:
        mov edx, [esp + 4]
        xor eax, eax
        in al, dx
        ret

    global io_read_u16
    io_read_u16:
        mov edx, [esp + 4]
        xor eax, eax
        in ax, dx
        ret

    global io_read_u32
    io_read_u32:
        mov edx, [esp + 4]
        in eax, dx
        ret

    global io_setup_gdt
    io_setup_gdt:
        mov ecx, [esp+4]
        dec ecx
        mov [0x6000], cx
        mov dword [0x6002], 0x6010
        lgdt [0x6000]
        jmp 8:.protected_start
        .protected_start:
        mov dx, 0x10
        mov ds, dx
        mov es, dx
        mov fs, dx
        mov gs, dx
        mov ss, dx
        ret

    global io_setup_idt
    io_setup_idt:
        mov ecx, [esp+4]
        dec ecx
        mov [0x7000], cx
        mov dword [0x7002], 0x7010
        lidt [0x7000]
        ret
    
    global io_enable_int
    io_enable_int:
        sti
        ret
    
    global io_disable_int
    io_disable_int:
        cli
        ret

    global io_enable_nmi
    io_enable_nmi:
        in al, 0x70
        and al, 0x7f
        out 0x70, al
        in al, 0x71
        ret
    
    global io_disable_nmi
    io_disable_nmi:
        in al, 0x70
        or al, 0x80
        out 0x70, al
        in al, 0x71
        ret

        