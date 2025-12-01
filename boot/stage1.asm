    org 0x7c00
    cpu 386
    bits 16
start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, start
    sti
    .try:
        push dx
        mov dh, 0
        mov cx, 2
        mov bx, stage2
        mov ax, 0x207
        int 0x13
        pop dx
        jnc .ok
        push dx
        xor ax, ax
        int 0x13
        pop dx
        jmp .try
    .ok:
    jmp stage2

    times 200-($-$$) db 0
    times 400-($-$$) db 0
    times 510-($-$$) db 0
    dw 0xaa55
stage2: