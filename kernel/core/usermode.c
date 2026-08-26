#include "usermode.h"

// Builds a fake interrupt return frame and executes iret into it.
// This is the ONLY way to drop CPU privilege on x86 - there's no "switch to ring 3" instruction.
void enter_usermode(void* entry, void* user_stack_top) {
    __asm__ __volatile__ (
        "cli\n"
        "mov $0x23, %%ax\n"   // user data selector (0x20) with RPL=3
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"

        "pushl $0x23\n"        // SS  - user stack segment
        "pushl %0\n"           // ESP - user stack pointer
        "pushf\n"
        "pop %%eax\n"
        "push %%eax\n"         // EFLAGS - unchanged, IF stays 0 from the earlier cli
        "pushl $0x1B\n"        // CS  - user code selector (0x18) with RPL=3
        "pushl %1\n"           // EIP - where ring 3 starts executing
        "iret\n"
        :
        : "r"(user_stack_top), "r"(entry)
        : "eax"
    );
}