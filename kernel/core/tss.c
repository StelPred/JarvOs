#include "tss.h"
#include <stddef.h>
#include "gdt.h"
// Standard x86 32-bit TSS layout. We only actively use esp0/ss0 -
// the rest exists because the CPU expects the full structure to be present.
struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;      // kernel stack pointer used on ring 3 -> ring 0 transition
    uint32_t ss0;        // kernel stack segment for the same transition
    uint32_t esp1, ss1, esp2, ss2;
    uint32_t cr3, eip, eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

static struct tss_entry tss;


static void gdt_set_tss_entry(uint32_t base, uint32_t limit) {
    uint64_t entry = 0;
    entry |= (uint64_t)(limit & 0xFFFF);
    entry |= (uint64_t)(base & 0xFFFFFF) << 16;
    entry |= (uint64_t)0x89 << 40;              // present, ring0, 32-bit TSS (available)
    entry |= (uint64_t)((limit >> 16) & 0xF) << 48;
    entry |= (uint64_t)((base >> 24) & 0xFF) << 56;
    gdt_get_table()[5] = entry;                      // index 5 = the placeholder we added
}

void tss_init(void) {
    for (size_t i = 0; i < sizeof(tss); i++) {
        ((uint8_t*)&tss)[i] = 0;
    }
    tss.ss0 = KERNEL_DS;
    tss.esp0 = 0;              // set per-process later via tss_set_kernel_stack
    tss.iomap_base = sizeof(tss);  // no I/O bitmap - ring 3 gets no direct port access

    gdt_set_tss_entry((uint32_t)&tss, sizeof(tss) - 1);

    __asm__ __volatile__("ltr %%ax" : : "a"(TSS_SEL));
}

void tss_set_kernel_stack(uint32_t esp0) {
    tss.esp0 = esp0;
}