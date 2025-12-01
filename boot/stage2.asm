    org 0x7e00
    cpu 386
    bits 16
section .bss
    _gdt_pointer: equ 0x6000
    _gdt_table: equ 0x6010
    _disk_id: resb 1
    _disk_sects: resw 1
    _disk_heads: resw 1
    _tty_enable_output: resb 1
    _video_width: resw 1
    _video_height: resw 1
    _video_bpp: resw 1
    _buffer: resb 512
    _kernel_dest: resd 1
    _kernel_pos: resd 1
section .data
    _path_system_dir: db "System",0
    _path_kernel: db "Kernel",0

section .text
    start:
        mov [_disk_id], dl
        mov dword [_kernel_dest], 0x100000
        mov dword [_kernel_pos], 0x100000
        cli
        xor ax, ax
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov sp, 0x7c00
        sti
        mov byte [_tty_enable_output], 1
        call debug_print_inline
        db "Tangerine System Software Loader",13,10,0
        call unreal_init
        call a20_init
        jc .a20_ok
            call debug_print_inline
            db "PANIC - Can't enable A20 Address Line.",0
            cli
            hlt
            jmp $
        .a20_ok:
        call disk_init
        call debug_print_inline
        db "Disk geometry: ",0
        xor eax, eax
        mov ax, [_disk_heads]
        call debug_print_u32
        call debug_print_inline
        db " heads ",0
        xor eax, eax
        mov ax, [_disk_sects]
        call debug_print_u32
        call debug_print_inline
        db " sectors",13,10,0
        call video_init
        call tfs_init
        xor eax, eax
        xor edx, edx
        mov ax, [_tfs_root_id]
        mov dx, [_tfs_root_id+2]
        mov edi, _buffer
        mov esi, _path_system_dir
        call tfs_find
        jnc fail
        mov edi, _buffer
        mov esi, _path_kernel
        call tfs_find
        jnc fail
        call debug_print_inline
        db "Loading Kernel",0
        mov edi, [_kernel_pos]
        and eax, 0xffff
        and edx, 0xff
        call tfs_read
        .load:
            push eax
            push edx
            call debug_print_inline
            db ".",0
            mov edi, [_kernel_pos]
            add edi, 512
            mov [_kernel_pos], edi
            pop edx
            pop eax
            call tfs_read_next
            jc .load
        call debug_print_inline
        db "ok",13,10,0
        mov esi, 0x7c00 + 100
        mov ecx, 300
        mov edi, [_kernel_dest]
        mov edi, [edi + 4]
        .copy_args:
            mov al, [esi]
            mov [edi], al
            inc esi
            inc edi
            loop .copy_args
        jmp prot_init
    start32:
        bits 32
        mov edi, [_kernel_dest]
        mov eax, [edi]
        jmp eax
        bits 16
    fail:
        call video_blink_fail
        jmp $


%include "src/a20.asm"
%include "src/unreal.asm"
%include "src/debug.asm"
%include "src/disk.asm"
%include "src/video.asm"
%include "src/tfs.asm"
%include "src/nmi.asm"
%include "src/prot.asm"