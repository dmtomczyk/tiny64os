.intel_syntax noprefix

.global long_mode_start
.extern kernel_main

.section .text
.code64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call kernel_main
1:
    hlt
    jmp 1b
