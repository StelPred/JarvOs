#include <stdint.h>
#include "../../lib/io.h"
#include "../hal.h"

// Keyboard controller ports
#define KB_DATA_PORT      0x60
#define KB_STATUS_PORT    0x64
#define KB_CMD_PORT       0x64

// Keyboard controller status flags
#define KB_STAT_OBF       0x01  // Output buffer full
#define KB_STAT_IBF       0x02  // Input buffer full
#define KB_STAT_SYS       0x04  // System flag
#define KB_STAT_CMD_DATA  0x08  // Command/data: 0=data, 1=command
#define KB_STAT_INHIBIT   0x10  // Inhibit switch
#define KB_STAT_TRAN_TIM  0x20  // Transmit timeout
#define KB_STAT_RECV_TIM  0x40  // Receive timeout
#define KB_STAT_ERROR     0x80  // Error

// Keyboard commands
#define KB_CMD_SET_LEDS   0xED
#define KB_CMD_ECHO       0xEE
#define KB_CMD_SCANCODE_SET 0xF0
#define KB_CMD_ENABLE     0xF4
#define KB_CMD_DISABLE    0xF5
#define KB_CMD_SET_DELAY  0xF6
#define KB_CMD_SET_TYPEMATIC 0xF7
#define KB_CMD_RESET      0xFF

// state tracking
static uint8_t shift_pressed = 0;
static uint8_t caps_lock_on = 0;

// Global variables for keyboard state (these will be used by the HAL)
// We expose these as extern in hal.c so the IRQ handler can update them
uint8_t keyboard_scan_code = 0;
uint8_t keyboard_key_available_flag = 0;

// Initialize keyboard controller
extern void debug_mark(int slot, char c, uint8_t color);

// Initialize keyboard controller
void hal_keyboard_init(void) {
    // Enable keyboard interface
    uint8_t status;

    // Wait for input buffer to be empty
    while (inb(KB_STATUS_PORT) & KB_STAT_IBF) {
        // Wait
    }

    // Write command to enable keyboard
    outb(KB_CMD_PORT, 0xAE);  // Enable keyboard interface

    // Wait for input buffer to be empty
    while (inb(KB_STATUS_PORT) & KB_STAT_IBF) {
        // Wait
    }

    // Read the configuration byte
    outb(KB_CMD_PORT, 0x20);  // Read command byte
    while (!(inb(KB_STATUS_PORT) & KB_STAT_OBF)) {
        // Wait for output buffer full
    }
    status = inb(KB_DATA_PORT);

    // Set bits: bit 0 = keyboard interrupt enable, bit 1 = mouse interrupt enable
    status |= 0x01;  // Enable keyboard interrupts
    status &= ~0x02; // Disable mouse interrupts (we don't have mouse)
    status &= ~0x10; // Disable keyboard clock (not sure if needed)

    // Wait for input buffer to be empty
    while (inb(KB_STATUS_PORT) & KB_STAT_IBF) {
        // Wait
    }

    // Write the configuration byte back
    outb(KB_CMD_PORT, 0x60);  // Write command byte
    while (inb(KB_STATUS_PORT) & KB_STAT_IBF) {
        // Wait
    }
    outb(KB_DATA_PORT, status);

    // Enable keyboard scanning
    while (inb(KB_STATUS_PORT) & KB_STAT_IBF) {
        // Wait
    }
    outb(KB_DATA_PORT, KB_CMD_ENABLE);   // <-- changed from KB_CMD_PORT
}

// Check if a key is available (non-blocking)
int hal_keyboard_key_available(void) {
    return keyboard_key_available_flag != 0;
}

// Read a key scancode (blocking if no key available)
uint8_t hal_keyboard_read_scancode(void) {
    // Wait for output buffer to be full
    while (!keyboard_key_available_flag) {
        // Wait
    }
    uint8_t scancode = keyboard_scan_code;
    keyboard_key_available_flag = 0; // Reset the flag
    return scancode;
}

// Simple polling function to wait for keypress and return ASCII
// This is a very basic implementation for demonstration
int hal_keyboard_getchar(void) {
    while (1) {
        while (!hal_keyboard_key_available()) {
            // Busy wait
        }

        uint8_t scancode = hal_keyboard_read_scancode();

        // Handle shift press/release
        if (scancode == 0x2A || scancode == 0x36) { shift_pressed = 1; continue; }
        if (scancode == 0xAA || scancode == 0xB6) { shift_pressed = 0; continue; }
        // Handle caps lock toggle (only on press)
        if (scancode == 0x3A) { caps_lock_on = !caps_lock_on; continue; }
        // Ignore all other break codes (key releases)
        if (scancode & 0x80) { continue; }

        char c;
        switch (scancode) {
            case 0x02: c = shift_pressed ? '!' : '1'; break;
            case 0x03: c = shift_pressed ? '@' : '2'; break;
            case 0x04: c = shift_pressed ? '#' : '3'; break;
            case 0x05: c = shift_pressed ? '$' : '4'; break;
            case 0x06: c = shift_pressed ? '%' : '5'; break;
            case 0x07: c = shift_pressed ? '^' : '6'; break;
            case 0x08: c = shift_pressed ? '&' : '7'; break;
            case 0x09: c = shift_pressed ? '*' : '8'; break;
            case 0x0A: c = shift_pressed ? '(' : '9'; break;
            case 0x0B: c = shift_pressed ? ')' : '0'; break;
            case 0x0C: c = shift_pressed ? '_' : '-'; break;
            case 0x0D: c = shift_pressed ? '+' : '='; break;
            case 0x0E: return '\b';
            case 0x0F: return '\t';
            case 0x10: c = 'q'; break;
            case 0x11: c = 'w'; break;
            case 0x12: c = 'e'; break;
            case 0x13: c = 'r'; break;
            case 0x14: c = 't'; break;
            case 0x15: c = 'y'; break;
            case 0x16: c = 'u'; break;
            case 0x17: c = 'i'; break;
            case 0x18: c = 'o'; break;
            case 0x19: c = 'p'; break;
            case 0x1A: c = shift_pressed ? '{' : '['; break;
            case 0x1B: c = shift_pressed ? '}' : ']'; break;
            case 0x1C: return '\n';
            case 0x1E: c = 'a'; break;
            case 0x1F: c = 's'; break;
            case 0x20: c = 'd'; break;
            case 0x21: c = 'f'; break;
            case 0x22: c = 'g'; break;
            case 0x23: c = 'h'; break;
            case 0x24: c = 'j'; break;
            case 0x25: c = 'k'; break;
            case 0x26: c = 'l'; break;
            case 0x27: c = shift_pressed ? ':' : ';'; break;
            case 0x28: c = shift_pressed ? '"' : '\''; break;
            case 0x29: c = shift_pressed ? '~' : '`'; break;
            case 0x2B: c = shift_pressed ? '|' : '\\'; break;
            case 0x2C: c = 'z'; break;
            case 0x2D: c = 'x'; break;
            case 0x2E: c = 'c'; break;
            case 0x2F: c = 'v'; break;
            case 0x30: c = 'b'; break;
            case 0x31: c = 'n'; break;
            case 0x32: c = 'm'; break;
            case 0x33: c = shift_pressed ? '<' : ','; break;
            case 0x34: c = shift_pressed ? '>' : '.'; break;
            case 0x35: c = shift_pressed ? '?' : '/'; break;
            case 0x39: return ' ';
            default: continue; // unknown scancode, keep waiting
        }

        // Apply letter case: caps lock and shift invert each other
        if (c >= 'a' && c <= 'z') {
            if (caps_lock_on ^ shift_pressed) c -= 32; // to uppercase
        }
        return c;
    }
}