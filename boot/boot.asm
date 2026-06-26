section .multiboot
align 4
    MAGIC    equ 0x1BADB002
    FLAGS    equ 0x00000004        ; добавить VBE
    CHECKSUM equ -(MAGIC + FLAGS)

    dd MAGIC
    dd FLAGS
    dd CHECKSUM

    ; VBE (графика)
    dd 0      ; mode type (0 = linear)
    dd 1024   ; width
    dd 768    ; height
    dd 32     ; bpp
section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top
    push ebx
    push eax
    call kernel_main
    cli
    hlt

section .bss
stack_bottom: resb 16384
stack_top: