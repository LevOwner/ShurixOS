; boot.asm – Multiboot загрузчик
section .multiboot
align 4
    MAGIC    equ 0x1BADB002
    FLAGS    equ 0x00000001        ; только выравнивание
    CHECKSUM equ -(MAGIC + FLAGS)

    dd MAGIC
    dd FLAGS
    dd CHECKSUM

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