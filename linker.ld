ENTRY(_start)

SECTIONS
{
    /* Базовый адрес для загрузки ядра (стандарт для Multiboot) */
    . = 1M;

    /* Секция Multiboot ДОЛЖНА идти самой первой! */
    .text :
    {
        KEEP(*(.multiboot))
        *(.text)
    }

    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(.bss)  *(COMMON) }
}
