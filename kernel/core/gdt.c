#include "gdt.h"
#include <stddef.h>

#define GDT_ENTRIES 6

static uint64_t gdt[GDT_ENTRIES];

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct gdt_ptr gp;

static void gdt_set_entry(int i, uint64_t value) {
    gdt[i] = value;
}

void gdt_init(void) {
    gdt_set_entry(0, 0x0000000000000000ULL); // null
    gdt_set_entry(1, 0x00CF9A000000FFFFULL);  // kernel code (0x08)
    gdt_set_entry(2, 0x00CF92000000FFFFULL);  // kernel data (0x10)
    gdt_set_entry(3, 0x00CFFA000000FFFFULL);  // user code   (0x1B)
    gdt_set_entry(4, 0x00CFF2000000FFFFULL);  // user data   (0x23)
    gdt_set_entry(5, 0x0000000000000000ULL);  // TSS - patched by tss_init (0x28)

    gp.limit = sizeof(gdt) - 1;
    gp.base = (uint32_t)&gdt;

    __asm__ __volatile__(
        "lgdt %0\n\t"
        "jmp $0x08, $reload_cs\n\t"
        "reload_cs:\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        :
        : "m"(gp)
        : "eax"
    );
}

uint64_t* gdt_get_table(void) {
    return gdt;
}