#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

// Interrupt Descriptor Table (IDT) entry
#define IDT_ENTRY_COUNT 256

// Gate types for IDT entries
#define IDT_TASK_GATE   0x5
#define IDT_INTERRUPT_GATE 0xE
#define IDT_TRAP_GATE   0xF

// IDT entry structure (packed)
struct idt_entry {
    uint16_t base_low;    // Lower 16 bits of handler address
    uint16_t sel;         // Kernel segment selector
    uint8_t  zero;        // Unused, always 0
    uint8_t  flags;       // Gate type, DPL, and present flag
    uint16_t base_high;   // Upper 16 bits of handler address
} __attribute__((packed));

// IDT pointer structure (for lidt instruction)
struct idt_ptr {
    uint16_t limit;       // Size of IDT - 1
    uint32_t base;        // Base address of IDT
} __attribute__((packed));

// Function prototypes
void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_install(void);

// External interrupt handlers (to be implemented in assembly or C)
extern void isr0(void);   // Timer (IRQ0)
extern void isr1(void);   // Keyboard (IRQ1)
// We'll add more as needed

#endif /* _IDT_H */