section .text
    nmi_enable:
        pushf
        push ax
        in al, 0x70
        and al, 0x7f
        out 0x70, al
        in al, 0x71
        pop ax
        popf
        ret
    nmi_disable:
        pushf
        push ax
        in al, 0x70
        or al, 0x80
        out 0x70, al
        in al, 0x71
        pop ax
        popf
        ret