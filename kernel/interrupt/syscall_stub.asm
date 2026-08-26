[bits 32]
global syscall_stub
extern syscall_handler
extern hal_serial_write

section .data
msg_a: db "A", 0
msg_b: db "B", 0
msg_c: db "C", 0

section .text
syscall_stub:
    pushad
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; checkpoint A
    push dword 1
    push dword msg_a
    call hal_serial_write
    add esp, 8

    push esp
    call syscall_handler
    add esp, 4

    ; checkpoint C
    push dword 1
    push dword msg_c
    call hal_serial_write
    add esp, 8

    pop gs
    pop fs
    pop es
    pop ds
    popad
    iretd