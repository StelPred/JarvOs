[org 0x7C00]

BOOT_DRIVE db 0

start:
    ; set video mode (IMPORTANT FIX)
    mov ax, 0x0003
    int 0x10

    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; save boot drive
    mov [BOOT_DRIVE], dl

    ; -------------------------
    ; load stage2 at 0x0000:0x7E00
    ; -------------------------
    xor ax, ax
    mov es, ax
    mov bx, 0x7E00

    mov ah, 0x02        ; BIOS read sectors
    mov al, 8           ; number of sectors
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]

    int 0x13
    jc disk_error

    ; jump to stage2
    jmp 0x0000:0x7E00


disk_error:
    mov ah, 0x0E
    mov al, 'F'
    int 0x10

hang:
    jmp hang

times 510-($-$$) db 0
dw 0xAA55