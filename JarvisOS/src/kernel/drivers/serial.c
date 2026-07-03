#include <stdint.h>
#include "../../lib/io.h"
#include "../hal.h"

// Serial port registers (for 16550 UART)
#define SERIAL_TRANSMIT_HOLDING_REG   0x00  // Write: Transmit Holding Register (THR)
#define SERIAL_RECEIVER_BUFFER_REG    0x00  // Read:  Receiver Buffer Register (RBR)
#define SERIAL_DIVISOR_LATCH_LSB      0x00  // Write: Divisor Latch Least Significant Bit (DLL)
#define SERIAL_INTERRUPT_ENABLE_REG   0x01  // Read/Write: Interrupt Enable Register (IER)
#define SERIAL_DIVISOR_LATCH_MSB      0x01  // Write: Divisor Latch Most Significant Bit (DLM)
#define SERIAL_INTERRUPT_IDENT_REG    0x02  // Read:  Interrupt Identification Register (IIR)
#define SERIAL_FIFO_CONTROL_REG       0x02  // Write: FIFO Control Register (FCR)
#define SERIAL_LINE_CONTROL_REG       0x03  // Read/Write: Line Control Register (LCR)
#define SERIAL_MODEM_CONTROL_REG      0x04  // Read/Write: Modem Control Register (MCR)
#define SERIAL_LINE_STATUS_REG        0x05  // Read:  Line Status Register (LSR)
#define SERIAL_MODEM_STATUS_REG       0x06  // Read:  Modem Status Register (MSR)
#define SERIAL_SCRATCH_REGISTER       0x07  // Read/Write: Scratch Register

// Line Control Register settings
#define SERIAL_LCR_DLAB               0x80  // Divisor Latch Access Bit
#define SERIAL_LCR_WLEN8              0x03  // Word Length: 8 bits
#define SERIAL_LCR_STOP1              0x00  // Stop Bits: 1
#define SERIAL_LCR_PARITY_NONE        0x00  // Parity: None

// Line Status Register settings
#define SERIAL_LSR_THRE               0x20  // Transmitter Holding Register Empty
#define SERIAL_LSR_DR                 0x01  // Data Ready

// Initialize serial port (COM1: 0x3F8)
void hal_serial_init(void) {
    uint16_t port = 0x3F8; // COM1
    uint16_t baud_divisor = 115200 / 9600; // 9600 baud

    // Turn off interrupts while setting divisors
    outb(port + SERIAL_INTERRUPT_ENABLE_REG, 0x00);

    // Set DLAB (divisor latch access bit) to 1 to allow setting baud rate
    outb(port + SERIAL_LINE_CONTROL_REG, SERIAL_LCR_DLAB);

    // Set least significant byte of the divisor
    outb(port + SERIAL_DIVISOR_LATCH_LSB, baud_divisor & 0xFF);
    // Set most significant byte of the divisor
    outb(port + SERIAL_DIVISOR_LATCH_MSB, (baud_divisor >> 8) & 0xFF);

    // Set line characteristics: 8 bits, no parity, 1 stop bit, and set DLAB to 0
    outb(port + SERIAL_LINE_CONTROL_REG, SERIAL_LCR_WLEN8 | SERIAL_LCR_STOP1 | SERIAL_LCR_PARITY_NONE);

    // Enable FIFO, clear TX and RX queues, and set interrupt watermark at 1 byte
    outb(port + SERIAL_FIFO_CONTROL_REG, 0xC7);

    // Tell the modem data terminal ready (DTR), request to send (RTS)
    outb(port + SERIAL_MODEM_CONTROL_REG, 0x0B);

    // Enable interrupts: we want to enable the received data interrupt (but we are not handling interrupts yet, so we leave it 0 for now)
    outb(port + SERIAL_INTERRUPT_ENABLE_REG, 0x01); // Enable received data interrupt

    // Alternatively, we can leave interrupts disabled for polling mode:
    // outb(port + SERIAL_INTERRUPT_ENABLE_REG, 0x00);
}

// Check if transmit holding register is empty (ready to accept another byte)
static int serial_is_transmit_empty(uint16_t port) {
    return inb(port + SERIAL_LINE_STATUS_REG) & SERIAL_LSR_THRE;
}

// Write a byte to the serial port
void serial_write_byte(uint16_t port, char c) {
    // Wait until the transmit holding register is empty
    while (!serial_is_transmit_empty(port)) {
        // We could do other work here, but for simplicity we just wait
    }
    outb(port + SERIAL_TRANSMIT_HOLDING_REG, c);
}

// Write a string to the serial port
void hal_serial_write(const char* data, size_t len) {
    uint16_t port = 0x3F8; // COM1
    for (size_t i = 0; i < len; i++) {
        serial_write_byte(port, data[i]);
    }
}