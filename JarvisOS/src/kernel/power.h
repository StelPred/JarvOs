#ifndef _POWER_H
#define _POWER_H

#include <stdint.h>

// CPU power states (C-states)
#define CPU_C0   0  // Normal operation
#define CPU_C1   1  // Halt
#define CPU_C2   2  // Stop Grant
#define CPU_C3   3  // Deep Sleep
#define CPU_C4   4  // Deeper Sleep

// CPU performance states (P-states) - we'll just use frequency in MHz for simplicity

// Initialize power management
void power_init(void);

// Set CPU C-state (power saving state)
// Returns 0 on success, -1 on error
int power_set_cpu_cstate(uint8_t state);

// Set CPU P-state (performance state - frequency)
// Returns 0 on success, -1 on error
int power_set_cpu_pstate(uint16_t mhz);

// Get current CPU frequency (in MHz)
uint16_t power_get_cpu_frequency(void);

// Enter sleep mode (system sleep)
// This would typically involve coordinating with chipset
void power_enter_sleep(void);

// ACPI-related functions (placeholders for future implementation)
int acpi_init(void);
int acpi_enable(void);
int acpi_disable(void);

#endif /* _POWER_H */