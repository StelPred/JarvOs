OUTPUT_FORMAT(binary)
ENTRY(start)
SECTIONS
{
    . = 0x00000000; /* Start at address 0 (for boot sector) */
    .text : {
        *(.multiboot_header)
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
}