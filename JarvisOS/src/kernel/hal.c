#include "hal.h"
#include "memory/pmm.h"
#include "memory/vmm.h"
#include "drivers/pic.h"
#include "drivers/pit.h"
#include "drivers/serial.h"
#include "drivers/keyboard.h"
#include "drivers/dma.h"
#include "power.h"

// Global tick counter (used by PIT)
uint64_t tick_count = 0;
static uint32_t current_hz = 100;
// Timer callback function
void (*timer_callback)(void) = NULL;
uint64_t hal_get_ticks(void) {
    return tick_count;
}
void hal_sleep_ms(uint32_t ms) {
    uint32_t ticks_needed = (ms * current_hz) / 1000;
    if (ticks_needed == 0) ticks_needed = 1;
    uint64_t target = hal_get_ticks() + ticks_needed;
    while (hal_get_ticks() < target) {
        __asm__ __volatile__("hlt");
    }
}
extern void debug_mark(int slot, char c, uint8_t color);

void hal_init(void) {
    hal_cpu_init();
    hal_memory_init();
    hal_interrupt_init();
    hal_timer_init();
    hal_serial_init();
    hal_keyboard_init();
    hal_dma_init();
    hal_power_init();

    pic_enable_irq(0);
    pic_enable_irq(1);
    __asm__ __volatile__("sti");
}


void hal_memory_init(void) {
    // Initialize physical and virtual memory managers
    // This will set up identity mapping for low memory
    vmm_init();
}

void hal_interrupt_init(void) {
    // Initialize the PIC
    pic_init();
    // Initialize the IDT
    idt_init();
}

void hal_timer_init(void) {
    // Initialize the PIT
    pit_init();
    // Set the timer callback
    pit_set_callback(NULL); // We'll handle timer ticks in the IRQ handler directly
}

// HAL memory management functions
void* hal_alloc_page(void) {
    return (void*)pmm_alloc_page();
}

void hal_free_page(void* addr) {
    pmm_free_page((uint32_t)addr);
}

// HAL timer functions
void hal_set_timer_frequency(uint32_t hz) {
    pit_set_frequency(hz);
    current_hz = hz;
}

// HAL DMA functions
int hal_dma_alloc_channel(void) {
    // For simplicity, we'll just return channel 0 for now.
    // In a real implementation, we would allocate a free channel.
    return 0;
}

void hal_dma_free_channel(int chan) {
    // For simplicity, we'll just do nothing for now.
    // In a real implementation, we would free the channel.
    (void)chan; // suppress unused parameter warning
}

// HAL power management functions
int hal_set_cpu_cstate(uint8_t state) {
    // For simplicity, we'll just return success for now.
    // In a real implementation, we would set the CPU power state.
    (void)state; // suppress unused parameter warning
    return 0;
}

// Initialize DMA controller
void hal_dma_init(void) {
    dma_init();
}

// Initialize power management
void hal_power_init(void) {
    // Placeholder for power management initialization
    // For now, do nothing
}