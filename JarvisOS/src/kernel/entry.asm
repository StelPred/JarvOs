[BITS 32]
global _start
extern kernel_main

_start:
    mov esp, 0x90000

    ; Copy .data section from ROM to RAM
    extern _sidata
    extern _sdata
    extern _edata
    mov esi, _sidata
    mov edi, _sdata
    mov ecx, _edata
    sub ecx, _sdata
    cld
    rep movsb

    ; Zero .bss
    extern __bss_start
    extern __bss_end
    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi
    xor eax, eax
    cld
    rep stosb

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang