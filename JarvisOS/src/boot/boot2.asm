[BITS 16]
[ORG 0x7E00]

jmp start

; =========================================================
; GDT
; =========================================================
align 8

gdt_start:
    dq 0x0000000000000000      ; null
    dq 0x00CF9A000000FFFF      ; code: base=0, limit=4GB (flat)
    dq 0x00CF92000000FFFF      ; data: base=0, limit=4GB (flat)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; =========================================================
; REAL MODE ENTRY
; =========================================================
start:
    cli
    cld

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; -------------------------
    ; ENABLE A20 FIRST
    ; -------------------------
    call enable_a20
    mov ah, 0x0E
    mov al, 'A'
    int 0x10
    ; -------------------------
    ; LOAD GDT
    ; -------------------------
    lgdt [gdt_descriptor]
    mov ah, 0x0E
    mov al, 'G'
    int 0x10
    ; -------------------------
    ; LOAD KERNEL (real mode, dl still holds boot drive from boot.asm)
    ; -------------------------
    mov ax, 0x1000
    mov es, ax
    xor bx, bx
    mov ah, 0x02
    mov al, 50
    mov ch, 0
    mov cl, 10
    mov dh, 0
    int 0x13
    jc disk_error2
    jmp load_kernel_done
disk_error2:
    cli
    hlt
    jmp $
load_kernel_done:
    ; -------------------------
    ; ENTER PROTECTED MODE SAFELY
    ; -------------------------
    cli

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:pm_entry          ; pm_entry is already the correct absolute address
; =========================================================
; PROTECTED MODE
; =========================================================
[BITS 32]
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, (50*512)/4
    rep movsd

    ; clear the screen so the kernel starts with a blank slate
    mov edi, 0xB8000
    mov ecx, 80*25
    mov ax, 0x0F20      ; space, white-on-black
    rep stosw

    jmp 0x08:0x100000
; =========================================================
; A20
; =========================================================
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret