[BITS 32]
; Global descriptor table selector for kernel code
%define KERNEL_CS 0x08
; Global descriptor table selector for kernel data
%define KERNEL_DS 0x10
extern exception_handler

; External function references for IRQ handlers (C functions)
extern irq0_handler
extern irq1_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern irq6_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler
extern irq16_handler
extern irq17_handler
extern irq18_handler
extern irq19_handler
extern irq20_handler
extern irq21_handler
extern irq22_handler
extern irq23_handler
extern irq24_handler
extern irq25_handler
extern irq26_handler
extern irq27_handler
extern irq28_handler
extern irq29_handler
extern irq30_handler
extern irq31_handler
extern irq32_handler
extern irq33_handler
extern irq34_handler
extern irq35_handler
extern irq36_handler
extern irq37_handler
extern irq38_handler
extern irq39_handler
extern irq40_handler
extern irq41_handler
extern irq42_handler
extern irq43_handler
extern irq44_handler
extern irq45_handler
extern irq46_handler
extern irq47_handler

; Macro to create ISR wrappers for exceptions that DON'T push error codes
%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    ; Push dummy error code to keep stack consistent with exceptions that do
    push dword 0
    ; Common ISR handling
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call exception_handler        ; Call the C function (isr0, isr1, etc.)
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 4          ; Remove dummy error code
    iretd
%endmacro

; Macro to create ISR wrappers for exceptions that DO push error codes
%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    ; CPU already pushed the error code, we don't push dummy
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call exception_handler          ; Call the C function (isr8, isr10, etc.)
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 4          ; Remove the error code
    iretd
%endmacro

; Now define which exceptions push error codes and which don't
; According to osdev.org, the following push error codes: 8, 10, 11, 12, 13, 14, 17
; Others do not.

; Exception wrappers without error code
ISR_NOERRCODE 0  ; Divide By Zero Exception
ISR_NOERRCODE 1  ; Debug
ISR_NOERRCODE 2  ; Non Maskable Interrupt
ISR_NOERRCODE 3  ; Breakpoint
ISR_NOERRCODE 4  ; Overflow
ISR_NOERRCODE 5  ; Bound Range Exceeded
ISR_NOERRCODE 6  ; Invalid Opcode
ISR_NOERRCODE 7  ; Device Not Available
ISR_ERRCODE  8  ; Double Fault (ERROR CODE!)
ISR_NOERRCODE 9  ; Coprocessor Segment Overrun
ISR_ERRCODE 10  ; Invalid TSS (ERROR CODE!)
ISR_ERRCODE 11  ; Segment Not Present (ERROR CODE!)
ISR_ERRCODE 12  ; Stack Segment Fault (ERROR CODE!)
ISR_ERRCODE 13  ; General Protection Fault (ERROR CODE!)
ISR_ERRCODE 14  ; Page Fault (ERROR CODE!)
ISR_NOERRCODE 15  ; Unknown Interrupt
ISR_NOERRCODE 16  ; Coprocessor Fault
ISR_ERRCODE 17  ; Alignment Check (ERROR CODE!)
ISR_NOERRCODE 18  ; Machine Check
ISR_NOERRCODE 19  ; SIMD Floating Point
ISR_NOERRCODE 20  ; Virtualization Exception
ISR_NOERRCODE 21  ; Control Protection Exception
ISR_NOERRCODE 22  ; Reserved
ISR_NOERRCODE 23  ; Reserved
ISR_NOERRCODE 24  ; Reserved
ISR_NOERRCODE 25  ; Reserved
ISR_NOERRCODE 26  ; Reserved
ISR_NOERRCODE 27  ; Reserved
ISR_NOERRCODE 28  ; Reserved
ISR_NOERRCODE 29  ; Reserved
ISR_NOERRCODE 30  ; Reserved
ISR_NOERRCODE 31  ; Reserved

; IRQ wrappers (32-47)
; IRQ wrappers (32-47)
global isr32
global isr33
global isr34
global isr35
global isr36
global isr37
global isr38
global isr39
global isr40
global isr41
global isr42
global isr43
global isr44
global isr45
global isr46
global isr47

isr32:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq0_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr33:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq1_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr34:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq2_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr35:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq3_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr36:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq4_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr37:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq5_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr38:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq6_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr39:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq7_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr40:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq8_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr41:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq9_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr42:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq10_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr43:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq11_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr44:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq12_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr45:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq13_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr46:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq14_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd

isr47:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq15_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iretd


; Function to load the IDT
global idt_load
idt_load:
    ; Load the IDT using the idt_ptr defined in idt.c
    extern idt_ptr
    lidt [idt_ptr]
    ret