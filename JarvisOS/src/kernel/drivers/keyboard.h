#ifndef _DRIVERS_KEYBOARD_H
#define _DRIVERS_KEYBOARD_H

// Initialize keyboard controller
void hal_keyboard_init(void);

// Check if a key is available (non-blocking)
int hal_keyboard_key_available(void);

// Read a key scancode (blocking if no key available)
uint8_t hal_keyboard_read_scancode(void);

#endif /* _DRIVERS_KEYBOARD_H */