#include "pic.h"
#include "../../lib/io.h"

// Small delay for I/O port synchronization
static void io_wait(void);

// Initialize the PICs
void pic_init(void) {
    // Start initialization sequence (in cascade mode)
    outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    io_wait();

    // ICW2: Vector offset
    outb(PIC1_DATA, 0x20);   // Master PIC: vector offset 0x20
    io_wait();
    outb(PIC2_DATA, 0x28);   // Slave PIC: vector offset 0x28
    io_wait();

    // ICW3: Tell Master PIC that there is a slave PIC at IRQ2 (0x04)
    // Tell Slave PIC its cascade identity (0x02)
    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();

    // ICW4: Set 8086/88 mode
    outb(PIC1_DATA, PIC_ICW4_8086);
    io_wait();
    outb(PIC2_DATA, PIC_ICW4_8086);
    io_wait();

    // Mask all interrupts for now
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

// Send an End-Of-Interrupt signal to the PIC
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        // Slave PIC
        outb(PIC2_COMMAND, PIC_EOI);
    }
    // Master PIC (whether slave or not)
    outb(PIC1_COMMAND, PIC_EOI);
}

// Enable a specific IRQ
void pic_enable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

// Disable a specific IRQ
void pic_disable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;

    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}