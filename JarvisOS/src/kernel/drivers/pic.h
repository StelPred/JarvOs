#ifndef _PIC_H
#define _PIC_H

#include <stdint.h>

// The PIC (Programmable Interrupt Controller) driver for 8259A

// I/O ports for the master and slave PICs
#define PIC1_COMMAND      0x20
#define PIC1_DATA         0x21
#define PIC2_COMMAND      0xA0
#define PIC2_DATA         0xA1

// Initialization Control Word (ICW) constants
#define PIC_ICW1_ICW4     0x01       // Indicates that ICW4 will be present
#define PIC_ICW1_INIT     0x10       // Initialization - required!
#define PIC_ICW4_8086     0x01       // 8086/88 (MCS-80/85) mode

// Command words
#define PIC_EOI           0x20       // End-of-interrupt command

// Initialize the PICs
void pic_init(void);

// Send an End-Of-Interrupt signal to the PIC
void pic_send_eoi(uint8_t irq);

// Enable a specific IRQ
void pic_enable_irq(uint8_t irq);

// Disable a specific IRQ
void pic_disable_irq(uint8_t irq);

// Remap the PIC interrupt offsets (to avoid conflict with exceptions)
void pic_remap(uint8_t offset1, uint8_t offset2);

#endif /* _PIC_H */