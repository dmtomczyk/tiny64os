.intel_syntax noprefix

.section .text

.extern isr_exception_handler
.extern irq0_handler
.extern irq1_handler

.global isr0
.global isr6
.global isr8
.global isr13
.global isr14
.global irq0
.global irq1

.macro PUSH_REGS
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
.endm

.macro POP_REGS
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
.endm

.macro ISR_NOERR num
isr\num:
    push 0
    push \num
    jmp isr_common
.endm

.macro ISR_ERR num
isr\num:
    push \num
    jmp isr_common
.endm

ISR_NOERR 0
ISR_NOERR 6
ISR_ERR 8
ISR_ERR 13
ISR_ERR 14

irq0:
    PUSH_REGS
    cld
    call irq0_handler
    POP_REGS
    iretq

irq1:
    PUSH_REGS
    cld
    call irq1_handler
    POP_REGS
    iretq

isr_common:
    PUSH_REGS
    mov rdi, [rsp + 120]
    mov rsi, [rsp + 128]
    lea rdx, [rsp + 136]
    cld
    call isr_exception_handler
    POP_REGS
    add rsp, 16
    iretq
