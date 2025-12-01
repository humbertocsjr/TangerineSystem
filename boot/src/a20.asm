section .text

    a20_init:
        pusha
        call a20_is_enabled
        jc .end
        call a20_test_bios_keyboard_support
        jc .keyboard_method
        call a20_test_bios_ioport_support
        jc .ioport_method
        mov ax, 0x2401
        int 0x15
        call a20_is_enabled
        jc .end
        .ioport_method:
            in al, 0x92
            test al, 2
            jnz .end
            or al, 2
            and al, 0xfe
            out 0x92, al
            call a20_is_enabled
            jnc .keyboard_method
        .end:
        popa
        call a20_is_enabled
        ret
        .keyboard_method:
            cli
            call .wait_bit_2
            mov al, 0xad
            out 0x64, al

            call .wait_bit_2
            mov al, 0xd0
            out 0x64, al

            call .wait_bit_1
            in al, (0x60)
            mov bx, ax

            call .wait_bit_2
            mov al, 0xd1
            out 0x64, al

            call .wait_bit_2
            mov ax, bx
            or ax, 2
            out 0x60, al

            call .wait_bit_2
            mov al, 0xae
            out 0x64, al

            jmp .end
        .wait_bit_1:
            in al, 0x64
            test al, 1
            jnz .wait_bit_1
            ret
        .wait_bit_2:
            in al, 0x64
            test al, 2
            jnz .wait_bit_1
            ret

    ; ret: cf = 1=ok | 0=nao suportado
    a20_test_bios_keyboard_support:
        pusha
        mov ax, 0x2403
        int 0x15
        jc .fail
            test ah, ah
            jnz .fail
            cmp bx, 0
            jne .fail
            stc
            jmp .end
        .fail:
            clc
        .end:
        popa
        ret

    ; ret: cf = 1=ok | 0=nao suportado
    a20_test_bios_ioport_support:
        pusha
        mov ax, 0x2403
        int 0x15
        jc .fail
            test ah, ah
            jnz .fail
            cmp bx, 1
            jne .fail
            stc
            jmp .end
        .fail:
            clc
        .end:
        popa
        ret

    ; ret: cf = 1=ativo | 0=inativo
    a20_is_enabled:
        push ax
        push si
        push di
        push es
        push ds
        cli
        xor ax, ax
        mov ds, ax
        dec ax
        mov es, ax
        mov si, 256
        mov di, 272
        push word [ds:si]
        push word [es:di]
        mov word [ds:si], 0xaaaa
        mov word [es:di], 0x5555
        mov ax, [es:di]
        cmp ax, [ds:si]
        je .fail
            stc
            jmp .end
        .fail:
            clc
        .end:
        pop word [es:di]
        pop word [ds:si]
        sti
        pop ds
        pop es
        pop di
        pop si
        pop ax
        ret