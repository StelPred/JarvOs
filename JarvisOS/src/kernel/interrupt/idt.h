#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

// IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;        // Kernel segment selector
    uint8_t  zero;       // Unused, always 0
    uint8_t  flags;      // Gate type, DPL, and P fields
    uint16_t base_high;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Function prototypes
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_install(void);
void idt_init(void);

#endif /* _IDT_H */