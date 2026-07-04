#include "idt.h"
#include "../../lib/io.h"
#include "../hal.h"

// IDT entries
static struct idt_entry idt_entries[IDT_ENTRY_COUNT];
// IDT pointer
struct idt_ptr idt_ptr;
// debug mark
extern void debug_mark(int slot, char c, uint8_t color);

// External interrupt handler prototypes (these will be linked from assembly wrappers)
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void isr32(void);
extern void isr33(void);
extern void isr34(void);
extern void isr35(void);
extern void isr36(void);
extern void isr37(void);
extern void isr38(void);
extern void isr39(void);
extern void isr40(void);
extern void isr41(void);
extern void isr42(void);
extern void isr43(void);
extern void isr44(void);
extern void isr45(void);
extern void isr46(void);
extern void isr47(void);

void exception_handler(void) {
    __asm__ __volatile__("cli; hlt");
}
// IRQ handler prototypes
extern void irq0_handler(void);
extern void irq1_handler(void);
extern void irq2_handler(void);
extern void irq3_handler(void);
extern void irq4_handler(void);
extern void irq5_handler(void);
extern void irq6_handler(void);
extern void irq7_handler(void);
extern void irq8_handler(void);
extern void irq9_handler(void);
extern void irq10_handler(void);
extern void irq11_handler(void);
extern void irq12_handler(void);
extern void irq13_handler(void);
extern void irq14_handler(void);
extern void irq15_handler(void);;

// Initialize the IDT
void idt_init(void) {
    // Set up the IDT pointer
    idt_ptr.limit = sizeof(struct idt_entry) * IDT_ENTRY_COUNT - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    // Clear the IDT
    for (int i = 0; i < IDT_ENTRY_COUNT; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Add ISRs for exceptions (0-31)
    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);  // Divide by zero
    idt_set_gate(1,  (uint32_t)isr1,  0x08, 0x8E);  // Debug
    idt_set_gate(2,  (uint32_t)isr2,  0x08, 0x8E);  // NMI
    idt_set_gate(3,  (uint32_t)isr3,  0x08, 0x8E);  // Breakpoint
    idt_set_gate(4,  (uint32_t)isr4,  0x08, 0x8E);  // Overflow
    idt_set_gate(5,  (uint32_t)isr5,  0x08, 0x8E);  // Bound range exceeded
    idt_set_gate(6,  (uint32_t)isr6,  0x08, 0x8E);  // Invalid opcode
    idt_set_gate(7,  (uint32_t)isr7,  0x08, 0x8E);  // Device not available
    idt_set_gate(8,  (uint32_t)isr8,  0x08, 0x8E);  // Double fault
    idt_set_gate(9,  (uint32_t)isr9,  0x08, 0x8E);  // Coprocessor segment overrun
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);  // Invalid TSS
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);  // Segment not present
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);  // Stack fault
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);  // General protection
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);  // Page fault
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);  // Unknown interrupt
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);  // Coprocessor fault
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);  // Alignment check
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);  // Machine check
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);  // SIMD floating point
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);  // Virtualization
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);  // Control protection
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);  // Reserved
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);  // Reserved
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);  // Reserved
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);  // Reserved
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);  // Reserved
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);  // Reserved
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);  // Reserved
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);  // Reserved
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);  // Reserved
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);  // Reserved

    // Add IRQ handlers (32-47)
    // IRQ0-7 (master PIC) map to IDT entries 32-39
    idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8E);  // IRQ0 (Timer)
    idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E);  // IRQ1 (Keyboard)
    idt_set_gate(34, (uint32_t)isr34, 0x08, 0x8E);  // IRQ2 (Cascade)
    idt_set_gate(35, (uint32_t)isr35, 0x08, 0x8E);  // IRQ3 (Serial port 2)
    idt_set_gate(36, (uint32_t)isr36, 0x08, 0x8E);  // IRQ4 (Serial port 1)
    idt_set_gate(37, (uint32_t)isr37, 0x08, 0x8E);  // IRQ5 (Parallel port 2)
    idt_set_gate(38, (uint32_t)isr38, 0x08, 0x8E);  // IRQ6 (Floppy disk)
    idt_set_gate(39, (uint32_t)isr39, 0x08, 0x8E);  // IRQ7 (Parallel port 1)

    // IRQ8-15 (slave PIC) map to IDT entries 40-47
    idt_set_gate(40, (uint32_t)isr40, 0x08, 0x8E);  // IRQ8 (CMOS clock)
    idt_set_gate(41, (uint32_t)isr41, 0x08, 0x8E);  // IRQ9 (Reserved)
    idt_set_gate(42, (uint32_t)isr42, 0x08, 0x8E); // IRQ10 (Reserved)
    idt_set_gate(43, (uint32_t)isr43, 0x08, 0x8E); // IRQ11 (Reserved)
    idt_set_gate(44, (uint32_t)isr44, 0x08, 0x8E); // IRQ12 (Mouse)
    idt_set_gate(45, (uint32_t)isr45, 0x08, 0x8E); // IRQ13 (Coprocessor)
    idt_set_gate(46, (uint32_t)isr46, 0x08, 0x8E); // IRQ14 (Hard disk)
    idt_set_gate(47, (uint32_t)isr47, 0x08, 0x8E); // IRQ15 (Reserved)
    // Load the IDT
    idt_install();
}

// Set an IDT gate
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].sel = sel;
    idt_entries[num].zero = 0;
    idt_entries[num].flags = flags;
}

// External function to load the IDT (defined in assembly)
extern void idt_load(void);

// Install the IDT
void idt_install(void) {
    idt_load();
}

// Assembly wrapper for ISRs - we'll define these in assembly
// These wrappers will save registers, call the C handler, and restore registers
// For now, we'll declare them as extern and expect them to be defined elsewhere

// Timer interrupt handler (IRQ0)
void irq0_handler(void) {
    outb(0x20, 0x20);
    extern uint64_t tick_count;
    tick_count++;
    extern void (*timer_callback)(void);
    if (timer_callback != NULL) {
        timer_callback();
    }

    // Call timer interrupt callback for debugging
    extern void timer_int_callback(void);
    timer_int_callback();
}

// Keyboard interrupt handler (IRQ1)
void irq1_handler(void) {
    // Send EOI to PIC
    outb(0x20, 0x20);

    // Read scan code from keyboard data port
    uint8_t scancode = inb(0x60);

    // Store the scan code in a buffer for the keyboard driver
    extern uint8_t keyboard_scan_code;
    keyboard_scan_code = scancode;

    // Signal that a key is available
    extern uint8_t keyboard_key_available_flag;
    keyboard_key_available_flag = 1;

    // Call keyboard interrupt callback for debugging
    extern void keyboard_int_callback(void);
    keyboard_int_callback();
}

// Other IRQ handlers (minimal implementations for now)
void irq2_handler(void) { outb(0x20, 0x20); }
void irq3_handler(void) { outb(0x20, 0x20); }
void irq4_handler(void) { outb(0x20, 0x20); }
void irq5_handler(void) { outb(0x20, 0x20); }
void irq6_handler(void) { outb(0x20, 0x20); }
void irq7_handler(void) { outb(0x20, 0x20); }
void irq8_handler(void) { outb(0xA0, 0x20); outb(0x20, 0x20); }
void irq9_handler(void) { outb(0xA0, 0x20); outb(0x20, 0x20); }
void irq10_handler(void) { outb(0xA0, 0x20); outb(0x20, 0x20); }
void irq11_handler(void) { outb(0xA0, 0x20); outb(0x20, 0x20); }
void irq12_handler(void) { outb(0xA0, 0x20); outb(0x20, 0x20); }
void irq13_handler(void) { outb(0xA0, 0x20); outb(0x20, 0x20); }
void irq14_handler(void) { outb(0xA0, 0x20); outb(0x20, 0x20); }
void irq15_handler(void) { outb(0xA0, 0x20); outb(0x20, 0x20); }