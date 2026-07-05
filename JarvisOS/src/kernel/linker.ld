ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : {
        _sidata = LOADADDR(.data);
        _sdata = .;
        *(.data*)
        _edata = .;
    }
    __bss_start = .;
    .bss : { *(.bss*) *(COMMON) }
    __bss_end = .;
}