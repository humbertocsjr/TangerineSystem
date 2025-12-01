section .text
    debug_print_char:
        pushf
        push ax
        out 0xe9, al
        cmp byte [_tty_enable_output], 1
        jne .end
        mov ah, 0xe
        int 0x10
        .end:
        pop ax
        popf
        ret

    bits 32
    debug32_print_inline:
        push ebp
        mov ebp, esp
        pushf
        pusha
        mov esi, [ebp+4]
        .loop:
            lodsb
            cmp al, 0
            je .end
            out 0xe9, al
            jmp .loop
        .end:
        mov [ebp+4], esi
        popa
        popf
        pop ebp
        ret
    bits 16

    debug_print_inline:
        push bp
        mov bp, sp
        pushf
        pusha
        push cs
        pop ds
        mov si, [bp+2]
        .loop:
            lodsb
            cmp al, 0
            je .end
            call debug_print_char
            jmp .loop
        .end:
        mov [bp+2], si
        popa
        popf
        pop bp
        ret
    
    debug_print_u32:
        pushf
        push eax
        push ebx
        push edx
        xor edx, edx
        mov ebx, 10
        div ebx
        cmp eax, 0
        je .end
        call debug_print_u32
        .end:
        mov al, dl
        add al, '0'
        call debug_print_char
        pop edx
        pop ebx
        pop eax
        popf
        ret