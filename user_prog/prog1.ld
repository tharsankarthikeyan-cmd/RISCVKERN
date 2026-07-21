
OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
    /* Set the user-space entry point */
    . = 0x1000;
    
    .text : {
        *(.text._start)
        *(.text .text.*)
    }
    
    /* Included for clean memory alignment */
    .rodata : { *(.rodata .rodata.*) }
    .data   : { *(.data .data.*) }
    .bss    : { *(.bss .bss.*) }
}
