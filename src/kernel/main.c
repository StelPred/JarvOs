#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "shell/commands.h"
#include "memory/vmm.h"
#include "memory/kmalloc.h"

// Global variables for interrupt counting (must be volatile as they're modified in IRQ handlers)
volatile uint32_t timer_ticks = 0;
volatile uint32_t keyboard_interrupts = 0;

// Simple VGA text mode driver for early debugging
static volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
static size_t vga_index = 0;
extern void debug_mark(int slot, char c, uint8_t color);
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
    volatile char* vga = (char*)0xB8000;
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
            vga[vga_index * 2]     = ' ';
            vga[vga_index * 2 + 1] = 0x0F;
        }
    } else {
        vga[vga_index * 2]     = c;
        vga[vga_index * 2 + 1] = 0x0F;
        vga_index++;
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

extern void hal_sleep_ms(uint32_t ms);

static void cmd_timer(const char *args) {
    (void)args;
    vga_puts("timer test - waiting 5 seconds\n");
    hal_sleep_ms(5000);
    vga_puts("timer done\n");
}

static void cmd_reboot(const char *args) {
    (void)args;
    vga_puts("reboot\n");
    outb(0x64, 0xFE);
}

// Command interpreter - simplified for debugging with non-blocking input
static void run_command_interpreter(void) {
    vga_puts("\n $ ");  // Changed prompt to make it obvious
    char buffer[64];
    int pos = 0;

    while (1) {
        // Non-blocking keyboard check
        if (hal_keyboard_key_available()) {
            int c = hal_keyboard_getchar();

            if (c == '\r' || c == '\n') {  // Enter
                buffer[pos] = '\0';
                vga_puts("\n");

                shell_dispatch(buffer);

                pos = 0;
                vga_puts(" $ ");
            } else if (c == '\b' && pos > 0) {
                pos--;
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            } else if (c >= 32 && c <= 126 && pos < 63) {
                buffer[pos++] = c;
                vga_putchar(c);
            }
        }

          
        // Halt if idle, waiting for interrupt
        __asm__ __volatile__("hlt");
    }
}

// Kernel entry point (called from boot2.asm after switching to protected mode)
void kernel_main(void) {
    extern int scheduler_init(void);
    hal_serial_write("STAGE1 scheduler_init\n", 22);
    scheduler_init();

    hal_serial_write("STAGE2 hal_init\n", 16);
    hal_init();

    hal_serial_write("STAGE2b vmm_init\n", 17);
    vmm_init();

    hal_serial_write("STAGE2c kmalloc_init\n", 21);
    kmalloc_init();

    hal_serial_write("STAGE3 timer\n", 13);
    hal_set_timer_frequency(100);

    hal_serial_write("STAGE5 commands\n", 16);
    shell_register_command("help", cmd_help, "Help");
    shell_register_command("mem", cmd_mem, "Memory test");
    shell_register_command("timer", cmd_timer, "Test");
    shell_register_command("reboot", cmd_reboot, "Reboot");

    hal_serial_write("STAGE6 interpreter\n", 19);
    run_command_interpreter();
}