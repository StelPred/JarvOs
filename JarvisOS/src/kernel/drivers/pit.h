#ifndef _PIT_H
#define _PIT_H

#include <stdint.h>

// The PIT (Programmable Interval Timer) driver for 8254

// I/O ports for the PIT
#define PIT_CHANNEL0_DATA   0x40
#define PIT_CHANNEL1_DATA   0x41
#define PIT_CHANNEL2_DATA   0x42
#define PIT_COMMAND_PORT    0x43

// PIT modes
#define PIT_MODE_INTERRUPT_ON_TERMINAL_COUNT   0x00  // Mode 0
#define PIT_MODE_HARDWARE_RETRIGGERABLE_ONESHOT 0x01  // Mode 1
#define PIT_MODE_RATE_GENERATOR                0x02  // Mode 2
#define PIT_MODE_SQUARE_WAVE_GENERATOR         0x03  // Mode 3
#define PIT_MODE_SOFTWARE_TRIGGERED_STROBE     0x04  // Mode 4
#define PIT_MODE_HARDWARE_TRIGGERED_STROBE     0x05  // Mode 5

// Access modes
#define PIT_ACCESS_LATCH_COUNT_VALUE           0x00  // Latch count value command
#define PIT_ACCESS_LOW_BYTE_ONLY               0x10  // Access low byte only
#define PIT_ACCESS_HIGH_BYTE_ONLY              0x20  // Access high byte only
#define PIT_ACCESS_LOW_BYTE_THEN_HIGH_BYTE     0x30  // Access low byte then high byte

// Initialize the PIT
void pit_init(void);

// Set the frequency of channel 0 (in Hz)
void pit_set_frequency(uint32_t frequency);

// Get the current tick count
uint64_t pit_get_ticks(void);

// Set the tick callback function
void pit_set_callback(void (*callback)(void));

#endif /* _PIT_H */