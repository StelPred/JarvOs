align 8

gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF   ; code
    dq 0x00CF92000000FFFF   ; data
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start