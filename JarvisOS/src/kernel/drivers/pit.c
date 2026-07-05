#include "pit.h"
#include "../../lib/io.h"
#include "../hal.h"

// Global tick counter
static volatile uint64_t tick_count = 0;
// Timer callback function
static void (*timer_callback)(void) = NULL;

// Initialize the PIT
void pit_init(void) {
    // Set default frequency to 100 Hz (10ms tick)
    pit_set_frequency(100);
}

// Set the frequency of channel 0 (in Hz)
void pit_set_frequency(uint32_t frequency) {
    // The PIT input clock is 1.193180 MHz
    uint32_t divisor = 1193180 / frequency;
    // Ensure we have a valid divisor
    if (divisor < 2) divisor = 2;
    if (divisor > 65535) divisor = 65535;

    // Send the command byte
    // Channel 0, low byte then high byte, mode 2 (rate generator)
    outb(PIT_COMMAND_PORT, 0x36); // 00 11 011 0 -> Channel 0, LSB/MSB, Mode 2

    // Send low byte
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);
    // Send high byte
    outb(PIT_CHANNEL0_DATA, (divisor >> 8) & 0xFF);
}

// Get the current tick count
uint64_t pit_get_ticks(void) {
    return tick_count;
}

// Set the tick callback function
void pit_set_callback(void (*callback)(void)) {
    timer_callback = callback;
}

// Timer interrupt handler (called from idt.c)
void pit_handler(void) {
    // Increment tick count
    tick_count++;

    // Call timer callback if set
    if (timer_callback != NULL) {
        timer_callback();
    }

    // Send End-of-Interrupt to PIC
    outb(0x20, 0x20);
}