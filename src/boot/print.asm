; Simple printing function for real mode (stage 1) and protected mode (stage 2)
; Uses BIOS interrupt 0x10 for teletype output
; Input: SI = pointer to null-terminated string
; Preserves: AX, BX, CX, DX, SI, DI
print_string:
    pusha
    mov ah, 0x0E      ; BIOS teletype function
.next:
    lodsb             ; Load byte from SI to AL, increment SI
    cmp al, 0
    jz .done
    int 0x10          ; BIOS interrupt
    jmp .next
.done:
    popa
    ret