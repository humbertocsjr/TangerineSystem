section .text
    video_init:
        mov byte [_tty_enable_output], 0
        call debug_print_inline
        db "Initializing VGA Mono",0
        mov ax, 0x11
        int 0x10
        call debug_print_inline
        db ".",0
        call video_draw_background
        call debug_print_inline
        db ".ok",13,10,0
        ret

    video_draw_background:
        pusha
        mov edi, 0xa0000
        mov ecx, 480/2
        .lines:
            push ecx
            mov ecx, 80
            .write_byte_1:
                mov byte [edi], 0xaa
                inc edi
                loop .write_byte_1
            pop ecx
            push ecx
            mov ecx, 80
            .write_byte_2:
                mov byte [edi], 0x55
                inc edi
                loop .write_byte_2
            pop ecx
            loop .lines
        popa
        ret

    video_blink_fail:
        call debug_print_inline
        db "SYSTEM HALTED",0
        .loop:
            call video_draw_background
            xor ax, ax
            int 0x1a
            mov bx, dx
            add bx, 18
            .wait:
                push bx
                xor ax, ax
                int 0x1a
                hlt
                pop bx
                cmp bx, dx
                ja .wait
            call video_draw_fail
            xor ax, ax
            int 0x1a
            mov bx, dx
            add bx, 18
            .wait_2:
                push bx
                xor ax, ax
                int 0x1a
                hlt
                pop bx
                cmp bx, dx
                ja .wait_2
        jmp .loop
    
    video_draw_fail:
        call video_draw_background
        mov esi, .image
        mov edi, 0xa0000 + ((40 * 480) - (.image_size/4 * 80)) + 36
        mov ecx, .image_size / 4
        .mask:
            push ecx
            mov ecx, 4
            .write_mask_byte:
                mov al, [esi]
                not al
                and al, [edi]
                mov [edi], al
                inc edi
                inc esi
                loop .write_mask_byte
            add edi, 76
            pop ecx
            loop .mask
        ret
        .image:
            db 0xff, 0x00, 0x00, 0xff
            db 0x7f, 0x80, 0x01, 0xfe
            db 0x3f, 0xc0, 0x03, 0xfc
            db 0x1f, 0xe0, 0x07, 0xf8
            db 0x0f, 0xf0, 0x0f, 0xf0
            db 0x07, 0xf8, 0x1f, 0xe0
            db 0x03, 0xfc, 0x3f, 0xc0
            db 0x01, 0xfe, 0x7f, 0x80
            db 0x00, 0xff, 0xff, 0x00
            db 0x00, 0x7f, 0xfe, 0x00
            db 0x00, 0x3f, 0xfc, 0x00
            db 0x00, 0x1f, 0xf8, 0x00
            db 0x00, 0x1f, 0xf8, 0x00
            db 0x00, 0x3f, 0xfc, 0x00
            db 0x00, 0x7f, 0xfe, 0x00
            db 0x00, 0xff, 0xff, 0x00
            db 0x01, 0xfe, 0x7f, 0x80
            db 0x03, 0xfc, 0x3f, 0xc0
            db 0x07, 0xf8, 0x1f, 0xe0
            db 0x0f, 0xf0, 0x0f, 0xf0
            db 0x1f, 0xe0, 0x07, 0xf8
            db 0x3f, 0xc0, 0x03, 0xfc
            db 0x7f, 0x80, 0x01, 0xfe
            db 0xff, 0x00, 0x00, 0xff
        .image_size: equ $ - .image