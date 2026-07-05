#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include "../include/kernel.h"

/* Physical memory management */
#define PAGE_SIZE 4096
#define PHYSICAL_MEMORY_START 0x100000   /* 1 MiB, after kernel and bootloader */
#define PHYSICAL_MEMORY_SIZE  (64 * 1024 * 1024) /* 64 MiB for simplicity */

/* Physical page frame structure */
typedef struct {
    uint32_t addr;   /* Physical address of the page frame */
    bool free;
} page_frame_t;

/* We'll manage a fixed array of page frames for simplicity */
#define MAX_PHYSICAL_PAGES (PHYSICAL_MEMORY_SIZE / PAGE_SIZE)

/* Functions for physical page management */
int mem_init(void);
void* mem_alloc_page(void);
void mem_free_page(void* addr);

/* Kernel heap management */
void* kmalloc(size_t size);
void kfree(void* ptr);

/* Optional: get the total free memory */
size_t mem_get_free_pages(void);

#endif /* KERNEL_MEMORY_H */