#include <stdint.h>
#include <stdbool.h>
#include "pmm.h"
#include "../../lib/io.h"

// We'll define the memory region we manage.
// In a real system, we would get this from the BIOS or boot loader.
#define MEMORY_START_ADDR 0x100000   // 1MB - after BIOS, boot loader, and kernel
#define MEMORY_END_ADDR   0x4000000  // 64MB - we assume we have up to 64MB of RAM
#define PAGE_SIZE         0x1000     // 4KB

// Bitmap to track free pages.
// Each bit represents a page: 1 = free, 0 = used.
// We'll allocate the bitmap statically in the .bss section.
// We need enough bits to cover the memory range we manage.
#define MANAGED_MEMORY (MEMORY_END_ADDR - MEMORY_START_ADDR)
#define NUMBER_OF_PAGES (MANAGED_MEMORY / PAGE_SIZE)
#define BITMAP_SIZE    (NUMBER_OF_PAGES / 8)  // in bytes, rounded up

// If NUMBER_OF_PAGES is not a multiple of 8, we need to round up.
#if (NUMBER_OF_PAGES % 8) != 0
#   define BITMAP_SIZE ((NUMBER_OF_PAGES / 8) + 1)
#endif

static uint8_t page_bitmap[BITMAP_SIZE];

// Total memory managed (in bytes)
static uint32_t total_memory = MANAGED_MEMORY;

// Initialize the physical memory manager
void pmm_init(void) {
    // Initially, all pages are free.
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        page_bitmap[i] = 0xFF;
    }

    // Now, mark the pages that are used by the kernel and boot loader as reserved.
    // We don't know the exact size of the kernel, but we can assume it's less than 2MB for now.
    // We'll reserve from 0x0 to 0x200000 (2MB) to be safe.
    // Note: our managed memory starts at 0x100000, so we need to mark from 0x100000 to 0x200000 as used.
    uint32_t kernel_end = 0x200000; // Assume kernel ends at 2MB

    // If the kernel end is above our managed memory start, we mark the pages from MEMORY_START_ADDR to kernel_end as used.
    if (kernel_end > MEMORY_START_ADDR) {
        uint32_t start_page = (MEMORY_START_ADDR - MEMORY_START_ADDR) / PAGE_SIZE; // 0
        uint32_t end_page   = (kernel_end - MEMORY_START_ADDR) / PAGE_SIZE;
        // Make sure we don't go beyond the managed memory
        if (end_page > NUMBER_OF_PAGES) {
            end_page = NUMBER_OF_PAGES;
        }
        for (uint32_t i = start_page; i < end_page; i++) {
            // Mark page i as used (set bit to 0)
            size_t byte_idx = i / 8;
            uint8_t bit_idx = i % 8;
            page_bitmap[byte_idx] &= ~(1 << bit_idx);
        }
    }
}

// Allocate a physical page
uint32_t pmm_alloc_page(void) {
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        if (page_bitmap[i] != 0x00) {
            // Find the first free bit in this byte
            for (int j = 0; j < 8; j++) {
                if (page_bitmap[i] & (1 << j)) {
                    // Mark this page as used
                    page_bitmap[i] &= ~(1 << j);
                    uint32_t page_num = i * 8 + j;
                    return MEMORY_START_ADDR + page_num * PAGE_SIZE;
                }
            }
        }
    }
    return 0; // No free pages
}

// Free a physical page
void pmm_free_page(uint32_t addr) {
    // Check if the address is within our managed memory
    if (addr < MEMORY_START_ADDR || addr >= MEMORY_END_ADDR) {
        return; // Ignore if out of range
    }

    uint32_t offset = addr - MEMORY_START_ADDR;
    uint32_t page_num = offset / PAGE_SIZE;

    if (page_num >= NUMBER_OF_PAGES) {
        return; // Invalid page number
    }

    // Mark the page as free
    size_t byte_idx = page_num / 8;
    uint8_t bit_idx = page_num % 8;
    page_bitmap[byte_idx] |= (1 << bit_idx);
}

// Mark a range of physical memory as reserved (unusable)
void pmm_mark_reserved(uint32_t start, uint32_t end) {
    // Clip the range to our managed memory
    if (end < MEMORY_START_ADDR) {
        return; // Entirely below managed memory
    }
    if (start >= MEMORY_END_ADDR) {
        return; // Entirely above managed memory
    }

    if (start < MEMORY_START_ADDR) {
        start = MEMORY_START_ADDR;
    }
    if (end > MEMORY_END_ADDR) {
        end = MEMORY_END_ADDR;
    }

    uint32_t start_page = (start - MEMORY_START_ADDR) / PAGE_SIZE;
    uint32_t end_page   = (end - MEMORY_START_ADDR) / PAGE_SIZE;

    // We want to mark [start_page, end_page) as used (so we clear the bits)
    for (uint32_t i = start_page; i < end_page; i++) {
        size_t byte_idx = i / 8;
        uint8_t bit_idx = i % 8;
        page_bitmap[byte_idx] &= ~(1 << bit_idx);
    }
}

// Get total memory managed (in bytes)
uint32_t pmm_get_total_memory(void) {
    return total_memory;
}

// Get free memory (in bytes)
uint32_t pmm_get_free_memory(void) {
    uint32_t free_pages = 0;
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        uint8_t byte = page_bitmap[i];
        // Count the number of set bits in the byte
        for (int j = 0; j < 8; j++) {
            if (byte & (1 << j)) {
                free_pages++;
            }
        }
    }
    return free_pages * PAGE_SIZE;
}