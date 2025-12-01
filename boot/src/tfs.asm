section .bss
    _tfs_root_id: resd 1

section .text
    tfs_init:
        xor dx, dx
        xor ax, ax
        mov edi, _buffer
        call disk_read
        jnc .end
        mov eax, [_buffer+412]
        mov [_tfs_root_id], eax
        stc
        .end:
        ret
    
    ; dl:ax = endereco
    ; edi = destino
    tfs_read:
        call disk_read
        ret
    
    ; dl:ax = endereco
    ; edi = destino
    ; ret: dl:ax = novo endereco
    tfs_read_next:
        push ebx
        push ecx
        push edi
        shl edx, 16
        and edx, 0x00ff0000
        and eax, 0x0000ffff
        or eax, edx
        mov ebx, 128
        xor edx, edx
        div ebx
        push edx
        mov edi, _buffer
        add eax, eax
        add eax, 8
        mov edx, eax
        shr edx, 16
        and dx, 0xff
        and ax, 0xffff
        call disk_read
        pop eax
        jnc .fail
        mov ebx, 4
        mul ebx
        add eax, _buffer
        inc eax
        mov ebx, eax
        mov ax, [ebx]
        mov dl, [ebx+2]
        xor dh, dh
        pop edi
        push edi
        cmp eax, 0
        je .fail
        call disk_read
        jmp .end
        .fail:
            xor dx, dx
            xor ax, ax
            clc
        .end:
        pop edi
        pop ecx
        pop ebx
        ret
    
    ; esi = nome do item
    ; dl:ax = diretorio
    ; ret: cf = 1=encontrado | 0=nao encontrado
    ; ret: dl:ax = endereco do item
    tfs_find:
        push ebx
        push ecx
        push edi
        push esi
        mov edi, _buffer
        call tfs_read
        jnc .not_found
        .try:
            push eax
            push edx
            push esi
            mov edi, _buffer
            cmp word [edi], 0
            je .try_next
            mov ecx, 4
            .loop:
                push ecx
                push esi
                push edi
                add edi, 64
                mov ecx, 64
                .compare:
                    mov al, [esi]
                    cmp al, [edi]
                    jne .not_equal
                    cmp al, 0
                    je .equal
                    inc esi
                    inc edi
                    loop .compare
                .equal:
                    pop esi ; descarta esi, com o conteudo anterior do edi
                    push esi ; salva o ponteiro atual
                    stc
                    jmp .end_compare
                .not_equal:
                    clc
                .end_compare:
                pop edi
                pop esi
                pop ecx
                jc .try_next
                pushf
                add edi, 128
                popf
                loop .loop
            .try_next:
            pop esi
            pop edx
            pop eax
            jc .found
            call tfs_read_next
            jnc .not_found
            jmp .try
        .not_found:
            xor eax, eax
            xor edx, edx
            clc
            jmp .end
        .found:
            mov ax, [edi+3]
            mov dl, [edi+5]
            and eax, 0xffff
            and edx, 0xff
            stc
        .end:
            pop esi
            pop edi 
            pop ecx
            pop ebx
            ret