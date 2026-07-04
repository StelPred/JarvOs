#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "shell/commands.h"

// Global variables for interrupt counting (must be volatile as they're modified in IRQ handlers)
volatile uint32_t timer_ticks = 0;
volatile uint32_t keyboard_interrupts = 0;

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

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void update_cursor(int pos) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_putchar(char c) {
    if (c == '\n') {
        // Move to start of next line
        size_t next_line_start = ((vga_index / 80) + 1) * 80;
        if (next_line_start >= 80 * 25) {
            // We're at the bottom - wrap to top
            vga_index = 0;
        } else {
            vga_index = next_line_start;
        }
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

// Keyboard interrupt callback (called from IRQ handler)
void keyboard_int_callback(void) {
    keyboard_interrupts++;
}

// Timer interrupt callback (called from IRQ handler)
void timer_int_callback(void) {
    timer_ticks++;
    // Show a visible timer tick every 10 interrupts in top-left corner
    if (timer_ticks % 10 == 0) {
        // Top-left corner character
        vga_buffer[0] = (vga_buffer[0] & 0xFF00) | ((timer_ticks / 10) % 16);
    }
}

// Simple command implementations (minimal for debugging)
static void cmd_help(const char *args) {
    (void)args;
    vga_puts("Help: mem timer reboot\n");
}

static void cmd_mem(const char *args) {
    (void)args;
    vga_puts("mem test\n");
    void* page = hal_alloc_page();
    if (page != NULL) {
        vga_puts("alloc ok\n");
        hal_free_page(page);
        vga_puts("free ok\n");
    } else {
        vga_puts("alloc FAIL\n");
    }
}

static void cmd_timer(const char *args) {
    (void)args;
    vga_puts("timer test\n");
    hal_set_timer_frequency(10);
    // Simple delay
    for (volatile int i = 0; i < 1000000; i++) {}
    hal_set_timer_frequency(100);
    vga_puts("timer done\n");
}

static void cmd_reboot(const char *args) {
    (void)args;
    vga_puts("reboot\n");
    outb(0x64, 0xFE);
}

// Command interpreter - simplified for debugging with non-blocking input
static void run_command_interpreter(void) {
    vga_puts("\nDBG> ");  // Changed prompt to make it obvious
    char buffer[16];
    int pos = 0;

    while (1) {
        // Non-blocking keyboard check
        if (hal_keyboard_key_available()) {
            int c = hal_keyboard_getchar();

            if (c == '\r' || c == '\n') {  // Enter
                buffer[pos] = '\0';
                vga_puts("\n");

                if (strcmp(buffer, "help") == 0) cmd_help(NULL);
                else if (strcmp(buffer, "mem") == 0) cmd_mem(NULL);
                else if (strcmp(buffer, "timer") == 0) cmd_timer(NULL);
                else if (strcmp(buffer, "reboot") == 0) cmd_reboot(NULL);
                else {
                    vga_puts("? ");
                    vga_puts(buffer);
                    vga_puts("\n");
                }

                pos = 0;
                vga_puts("DBG> ");
            } else if (c == '\b' && pos > 0) {
                pos--;
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            } else if (c >= 32 && c <= 126 && pos < 15) {
                buffer[pos++] = c;
                vga_putchar(c);
            }
        }

        // Show heartbeat in bottom-right corner every 50 timer ticks
        static uint32_t last_heartbeat = 0;
        if (timer_ticks - last_heartbeat >= 50) {
            last_heartbeat = timer_ticks;
            // Bottom-right corner
            size_t pos = 80 * 25 - 1;
            vga_buffer[pos] = (vga_buffer[pos] & 0xFF00) | (((timer_ticks / 50) % 10) + '0');
        }

        // Halt if idle, waiting for interrupt
        __asm__ __volatile__("hlt");
    }
}

// Kernel entry point (called from boot2.asm after switching to protected mode)
void kernel_main(void)
{
    // STAGE 1: VGA TEST
    vga_puts("STAGE1: VGA ok\n");
    for (int i = 0; i < 10; i++) {
        vga_putchar('0' + i);
    }
    vga_puts("\n");

    // STAGE 2: HAL INIT
    vga_puts("STAGE2: HAL init\n");
    hal_init();
    vga_puts("STAGE2: HAL done\n");

    // STAGE 3: TIMER SETUP
    vga_puts("STAGE3: Timer setup\n");
    hal_set_timer_frequency(100);  // 100 Hz
    vga_puts("STAGE3: Timer done\n");

    // STAGE 4: KEYBOARD SETUP
    vga_puts("STAGE4: Keyboard setup\n");
    hal_keyboard_init();
    vga_puts("STAGE4: Keyboard done\n");

    // STAGE 5: COMMAND REGISTRATION
    vga_puts("STAGE5: Commands\n");
    shell_register_command("help", cmd_help, "Help");
    shell_register_command("mem", cmd_mem, "Memory test");
    shell_register_command("timer", cmd_timer, "Timer test");
    shell_register_command("reboot", cmd_reboot, "Reboot");
    vga_puts("STAGE5: Commands done\n");

    // STAGE 6: ENTER INTERPRETER
    vga_puts("STAGE6: Entering interpreter\n");
    run_command_interpreter();
}