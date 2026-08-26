#ifndef _DRIVERS_SERIAL_H
#define _DRIVERS_SERIAL_H

#include <stddef.h>

// Initialize serial port (COM1: 0x3F8)
void hal_serial_init(void);

// Write a string to the serial port
void hal_serial_write(const char* data, size_t len);

#endif /* _DRIVERS_SERIAL_H */