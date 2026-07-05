[BITS 32]
global context_switch
context_switch:
    push ebp
    push ebx
    push esi
    push edi

    mov eax, [esp+20]      ; pointer to where we save the old stack position
    mov [eax], esp         ; save it

    mov esp, [esp+24]      ; load the new process's stack position

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret