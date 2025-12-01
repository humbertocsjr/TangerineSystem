section .text
    disk_init:
        mov al, [_disk_id]
        bt ax, 7
        jc disk_init_hard_disk
        jmp disk_init_floppy 

    disk_init_floppy:
        call debug_print_inline
        db "Testing floppy type",0
        mov word [_disk_heads], 2
        mov word [_disk_sects], 36
        call .try
        jc .end
        mov word [_disk_sects], 18
        call .try
        jc .end
        mov word [_disk_sects], 15
        call .try
        jc .end
        mov word [_disk_sects], 9
        call .try
        jc .end
        .end:
        call debug_print_inline
        db "ok",13,10,0
        ret
        .try:
            call debug_print_inline
            db ".",0
            mov ax, 0x201
            mov bx, _buffer
            mov cx, [_disk_sects]
            mov dh, [_disk_heads]
            dec dh
            mov dl, [_disk_id]
            int 0x13
            jnc .ok
            xor ax, ax
            mov dl, [_disk_id]
            int 0x13
            jmp .try
            .ok:
            ret
    
    disk_init_hard_disk:
        mov word [_disk_heads], 0
        mov word [_disk_sects], 0
        call debug_print_inline
        db "Reading disk geometry.",0
        mov di, 0
        mov ax, 0x800
        mov dl, [_disk_id]
        int 0x13
        mov [_disk_heads], dh
        inc word [_disk_heads]
        mov [_disk_sects], cl
        and word [_disk_sects], 0x3f
        call debug_print_inline
        db "ok",13,10,0
        ret

    ; dl:ax = endereco \
    ; edi = destino
    ; ret: cf = 1=sucesso | 0=falha
    disk_read:
        pusha
        shl edx, 16
        and edx, 0x00ff0000
        and eax, 0x0000ffff
        or eax, edx
        xor ebx, ebx
        mov bx, [_disk_sects]
        xor edx, edx
        div ebx
        mov cl, dl
        inc cl
        mov bx, [_disk_heads]
        xor edx, edx
        div ebx
        mov dh, dl
        mov ch, al
        shr eax, 2
        and eax, 0xc0
        or cl, al
        mov bx, _buffer
        mov ax, 0x201
        mov dl, [_disk_id]
        mov si, 5
        .try:
            pusha
            int 0x13
            popa
            jnc .ok
            pusha
            xor eax, eax
            mov dl, [_disk_id]
            int 0x13
            popa
            dec si
            jnz .try
            clc
            jmp .end
        .ok:
        mov esi, _buffer
        cmp esi, edi
        je .end_copy
        mov ecx, 512
        .copy:
            mov al, [esi]
            mov [edi], al
            inc esi
            inc edi
            loop .copy
        .end_copy:
        stc
        .end:
        popa
        ret