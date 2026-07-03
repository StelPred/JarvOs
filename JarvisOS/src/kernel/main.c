#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Simple VGA text mode driver for early debugging
static volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
static size_t vga_index = 0;

// Serial port for debugging
extern void hal_serial_init(void);
extern void hal_serial_write(const char* data, size_t len);

// Keyboard
extern void hal_keyboard_init(void);
extern int hal_keyboard_getchar(void);

// HAL functions
extern void hal_init(void);
extern void* hal_alloc_page(void);
extern void hal_free_page(void* addr);
extern void hal_set_timer_frequency(uint32_t hz);
extern uint64_t hal_get_ticks(void);
extern int hal_keyboard_key_available(void);
extern int hal_dma_alloc_channel(void);
extern int hal_set_cpu_cstate(uint8_t state);
extern const char* hal_get_cpu_vendor(void);
extern uint32_t hal_get_cpu_features(void);
extern void hal_dma_free_channel(int chan);

// PIT functions
extern void pit_set_callback(void (*callback)(void));

// I/O port functions
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void update_cursor(int pos) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
void vga_putchar(char c) {
    if (c == '\n') {
        vga_index = (vga_index / 80 + 1) * 80;
    } else if (c == '\b') {  // Backspace
        if (vga_index > 0) {
            vga_index--;
            vga_buffer[vga_index] = (uint16_t)' ' | 0x0F00;  // Clear character
        }
    } else {
        vga_buffer[vga_index++] = (uint16_t)c | 0x0F00;  // White on black
        // Wrap around if we hit the bottom
        if (vga_index >= 80 * 25) {
            vga_index = 0;
        }
    }
    update_cursor(vga_index);
}


void vga_puts(const char* s) {
    while (*s) {
        vga_putchar(*s++);
    }
}

void vga_puthex(uint32_t num) {
    vga_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        char c = "0123456789ABCDEF"[(num >> i) & 0xF];
        vga_putchar(c);
    }
}

void debug_mark(int slot, char c, uint8_t color) {
    volatile char* vga = (char*)0xB8000;
    int offset = (80 * 24 + slot) * 2;   // bottom row, column = slot
    vga[offset]     = c;
    vga[offset + 1] = color;
}

// Timer callback function to demonstrate periodic interrupts
static void timer_callback(void) {
    static int dots = 0;
    vga_putchar('.');
    dots++;
    if (dots >= 80) {  // New line after 80 dots
        vga_putchar('\n');
        dots = 0;
    }
}

// Simple command interpreter
static void run_command_interpreter(void) {
    vga_puts("\n $ ");
    char buffer[128];
    int pos = 0;

    while (1) {
        // Check for keypress
        int c = hal_keyboard_getchar();

            if (c == '\r' || c == '\n') {  // Enter key
                buffer[pos] = '\0';  // Null terminate
                vga_puts("\n");

                // Process command
                if (strcmp(buffer, "help") == 0) {
                    vga_puts("Available commands:\n");
                    vga_puts("  help - Show this help\n");
                    vga_puts("  mem - Test memory allocation\n");
                    vga_puts("  timer - Test timer frequency\n");
                    vga_puts("  power [0-4] - Set CPU C-state\n");
                    vga_puts("  clear - Clear screen\n");
                    vga_puts("  reboot - Reboot system (QEMU specific)\n");
                } else if (strcmp(buffer, "mem") == 0) {
                    vga_puts("Testing memory allocation...\n");
                    void* page = hal_alloc_page();
                    if (page != NULL) {
                        vga_puts("Allocated page at 0x");
                        vga_puthex((uint32_t)page);
                        vga_puts("\n");

                        // Write test pattern
                        for (int i = 0; i < 64; i++) {
                            ((char*)page)[i] = 'A' + (i % 26);
                        }
                        ((char*)page)[64] = '\0';

                        vga_puts("Page contents: ");
                        vga_puts((char*)page);
                        vga_puts("\n");

                        hal_free_page(page);
                        vga_puts("Page freed.\n");
                    } else {
                        vga_puts("Failed to allocate page!\n");
                    }
                } else if (strcmp(buffer, "timer") == 0) {
                    vga_puts("Testing timer...\n");
                    hal_set_timer_frequency(10);  // 10 Hz
                    vga_puts("Timer set to 10 Hz for 5 seconds...\n");

                    extern void hal_sleep_ms(uint32_t ms);
                    hal_sleep_ms(5000);

                    hal_set_timer_frequency(100);  // Back to 100 Hz
                    vga_puts("Timer test complete.\n");

                } else if (strncmp(buffer, "power ", 6) == 0) {
                    int state = buffer[6] - '0';
                    if (state >= 0 && state <= 4) {
                        vga_puts("Setting CPU C-state to ");
                        vga_putchar('0' + state);
                        vga_puts("...\n");
                        int result = hal_set_cpu_cstate(state);
                        if (result == 0) {
                            vga_puts("CPU C-state set successfully.\n");
                        } else {
                            vga_puts("Failed to set CPU C-state!\n");
                        }
                    } else {
                        vga_puts("Invalid power state. Use 0-4.\n");
                    }
                } else if (strcmp(buffer, "clear") == 0) {
                    for (size_t i = 0; i < 80 * 25; i++) {
                        vga_buffer[i] = 0x0F00 | ' ';
                    }
                    vga_index = 0;
                } else if (strcmp(buffer, "reboot") == 0) {
                    vga_puts("Rebooting...\n");
                    // Reboot via keyboard controller (works in QEMU/Bochs)
                    outb(0x64, 0xFE);
                    // Should not return
                } else if (buffer[0] != '\0') {
                    vga_puts("Unknown command: '");
                    vga_puts(buffer);
                    vga_puts("'. Type 'help' for available commands.\n");
                }

                // Reset for next command
                pos = 0;
                vga_puts(" $ ");
            } else if (c == '\b' && pos > 0) {  // Backspace
                pos--;
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            } else if (c >= 32 && c <= 126 && pos < 127) {  // Printable ASCII
                buffer[pos++] = c;
                vga_putchar(c);
            }
    }
}

// Kernel entry point (called from boot2.asm after switching to protected mode)
void kernel_main(void)
{
    hal_init();
    // (future) idt_init();
    // debug_mark(2, 'I', 0x0B);

    // (future) pmm_init(...);
    // debug_mark(3, 'P', 0x0C);

    run_command_interpreter();
}