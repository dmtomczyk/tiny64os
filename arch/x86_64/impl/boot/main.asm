.intel_syntax noprefix

.global start
.extern long_mode_start

.section .text
.code32
start:
    mov esp, OFFSET FLAT:stack_top

    call check_multiboot
    call check_cpuid
    call check_long_mode

    call setup_page_tables
    call enable_paging

    lgdt [gdt64_pointer]
    ljmp gdt64_code_segment, long_mode_start
1:
    hlt
    jmp 1b

check_multiboot:
    cmp eax, 0x36d76289
    jne no_multiboot
    ret
no_multiboot:
    mov al, 'M'
    jmp error

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je no_cpuid
    ret
no_cpuid:
    mov al, 'C'
    jmp error

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz no_long_mode

    ret
no_long_mode:
    mov al, 'L'
    jmp error

setup_page_tables:
    mov eax, OFFSET FLAT:page_table_l3
    or eax, 0b11
    mov dword ptr [page_table_l4], eax
    mov dword ptr [page_table_l4 + 4], 0

    mov eax, OFFSET FLAT:page_table_l2
    or eax, 0b11
    mov dword ptr [page_table_l3], eax
    mov dword ptr [page_table_l3 + 4], 0

    xor ecx, ecx
setup_page_tables_loop:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov dword ptr [page_table_l2 + ecx * 8], eax
    mov dword ptr [page_table_l2 + ecx * 8 + 4], 0

    inc ecx
    cmp ecx, 512
    jne setup_page_tables_loop

    ret

enable_paging:
    mov eax, OFFSET FLAT:page_table_l4
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

error:
    mov dword ptr [0xb8000], 0x4f524f45
    mov dword ptr [0xb8004], 0x4f3a4f52
    mov dword ptr [0xb8008], 0x4f204f20
    mov byte ptr [0xb800a], al
2:
    hlt
    jmp 2b

.section .bss
.align 4096
page_table_l4:
    .skip 4096
page_table_l3:
    .skip 4096
page_table_l2:
    .skip 4096
stack_bottom:
    .skip 4096 * 4
stack_top:

.section .rodata
.align 8
gdt64:
    .quad 0
.set gdt64_code_segment, . - gdt64
    .quad (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
gdt64_pointer:
    .short . - gdt64 - 1
    .quad gdt64
