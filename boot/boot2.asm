[BITS 16]
[ORG 0x7E00]
jmp start

; =========================================================
; GDT
; =========================================================
align 8
global gdt_start
gdt_start:
    dq 0x0000000000000000      ; null                       - index 0
    dq 0x00CF9A000000FFFF      ; kernel code, ring0, flat    - index 1 (selector 0x08)
    dq 0x00CF92000000FFFF      ; kernel data, ring0, flat    - index 2 (selector 0x10)
    dq 0x00CFFA000000FFFF      ; user code,   ring3, flat    - index 3 (selector 0x1B)
    dq 0x00CFF2000000FFFF      ; user data,   ring3, flat    - index 4 (selector 0x23)
    dq 0x0000000000000000      ; TSS - patched at runtime    - index 5 (selector 0x28)
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

KERNEL_SECTORS equ 100

; Disk Address Packet for INT 13h/AH=42h
dap:
    db 0x10          ; size of this packet (16 bytes)
    db 0              ; reserved
    dw KERNEL_SECTORS ; number of sectors to read
    dw 0x0000         ; destination offset
    dw 0x1000         ; destination segment  (0x1000:0x0000 = same target as before)
    dq 9              ; starting LBA sector (0-indexed! sector 10 in 1-indexed CHS = LBA 9)

cur_head: db 0
cur_cyl:  db 0
cur_sect: db 0
this_read: db 0
sectors_left: dw 0

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
    ; LOAD KERNEL (CHS, chunked to respect 18 sectors/track limit)
    ; -------------------------
    mov ax, 0x1000
    mov es, ax
    xor bx, bx
    mov byte [cur_head], 0
    mov byte [cur_cyl], 0
    mov byte [cur_sect], 10   ; starting sector (1-indexed)
    mov word [sectors_left], KERNEL_SECTORS

read_loop:
    cmp word [sectors_left], 0
    je load_kernel_done

    mov ah, 0x0E
    mov al, [this_read]
    add al, '0'
    int 0x10
    mov al, [cur_sect]
    add al, '0'
    int 0x10
    mov al, [cur_head]
    add al, '0'
    int 0x10
    mov al, '|'
    int 0x10

    ; sectors available on this track from cur_sect to 18
    mov al, 18
    sub al, [cur_sect]     ; al = 18 - 10 = 8
    inc al                  ; al = 9
    cmp al, [sectors_left]
    jle .use_track_max
    mov al, byte [sectors_left]
.use_track_max:
    mov byte [this_read], al

    mov ah, 0x02
    mov al, [this_read]
    mov ch, [cur_cyl]
    mov cl, [cur_sect]
    mov dh, [cur_head]
    mov dl, 0x80
    int 0x13
    jc disk_error2

    ; advance destination pointer: bx += this_read * 512
    xor ah, ah
    mov al, [this_read]
    mov dx, 512
    mul dx              ; ax = this_read * 512
    add bx, ax

    ; sectors_left -= this_read
    xor ah, ah
    mov al, [this_read]
    sub [sectors_left], ax

    ; advance to next track: cur_sect=1, cur_head++ (wrap to cur_cyl++ after head 1)
    mov byte [cur_sect], 1
    mov al, [cur_head]
    inc al
    cmp al, 2
    jl .head_ok
    xor al, al
    inc byte [cur_cyl]
.head_ok:
    mov [cur_head], al

    jmp read_loop

disk_error2:
    mov ah, 0x0E
    mov al, 'X'
    int 0x10
    cli
    hlt
    jmp $

load_kernel_done:
    ; Reset ES register to 0 to ensure correct memory addressing
    xor ax, ax
    mov es, ax

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
    mov ecx, (KERNEL_SECTORS*512)/4
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