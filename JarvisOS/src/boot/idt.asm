[BITS 32]
; Simple IDT setup for bootloader
; This sets up a minimal IDT for use during boot process

%define KERNEL_CS 0x08   ; Kernel code segment selector from GDT
%define KERNEL_DS 0x10   ; Kernel data segment selector from GDT

; IDT entry structure
; Offset: 0-15  : Low 16 bits of handler address
; Offset: 16-31 : Segment selector
; Offset: 32-39 : Zero (5 bits) + Flags (3 bits) + Zero (1 bit) + DPL (2 bits) + P (1 bit)
; Offset: 40-47 : Zero (for 32-bit interrupt gate)
; Offset: 48-63 : High 16 bits of handler address

; IDT pointer structure
; Offset: 0-15 : Limit (size of IDT - 1)
; Offset: 16-47: Base address of IDT

section .rodata
align 8
idt_ptr:
    dw 0x0000   ; limit (will be set in code)
    dd 0x00000000 ; base (will be set in code)

; IDT entries - we'll create a small IDT with just a few essential entries
; For now, we'll set up 256 entries (standard size) but only initialize a few
align 8
idt_entries:
    times 256 dw 0x0000    ; offset low
    times 256 dw 0x0000    ; selector
    times 256 db 0x00      ; zero
    times 256 db 0x00      ; flags
    times 256 dw 0x0000    ; offset high

section .text
global idt_init
; Removed extern panic - we'll handle exceptions locally in the bootloader

; Initialize the IDT with basic handlers
idt_init:
    pusha

    ; Set up IDT pointer
    mov eax, idt_entries
    mov [idt_ptr + 2], eax   ; base address

    mov word [idt_ptr + 0], 256 * 8 - 1 ; limit (256 entries * 8 bytes each - 1)

    ; Clear all IDT entries
    mov ecx, 256           ; number of entries
    mov edi, idt_entries   ; destination
    xor eax, eax           ; clear value
    rep stosd              ; clear 4 bytes at a time (each entry is 8 bytes, so we'll do this twice)
    mov ecx, 256           ; reset counter
    rep stosd              ; clear remaining 4 bytes of each entry

    ; Set up a basic exception handler for divide by zero (exception 0)
    ; This is just to catch any early errors
    mov eax, idt_entries
    mov [eax + 0*8 + 0], word isr0_stub    ; offset low
    mov [eax + 0*8 + 2], word KERNEL_CS    ; selector
    mov [eax + 0*8 + 4], byte 0            ; zero
    mov [eax + 0*8 + 5], byte 0x8E         ; flags: present, ring0, interrupt gate
    mov [eax + 0*8 + 6], word 0            ; offset high (zero for now)

    ; We could set up more handlers, but for a basic bootloader,
    ; we'll just halt on any exception

    popa
    ret

; Stub ISR that just halts (instead of calling external panic)
isr0_stub:
    pusha
    ; Simple infinite loop on exception
.hang:
    hlt
    jmp .hang
    popa
    iret

; Function to load the IDT
global idt_load
idt_load:
    ; Load the IDT using the idt_ptr defined above
    lidt [idt_ptr]
    ret