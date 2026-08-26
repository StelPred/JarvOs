#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>
#include <stddef.h>

// Physical Memory Manager
// Manages allocation and tracking of physical RAM

// Initialize the physical memory manager
// Called early in kernel startup after detecting available memory
void pmm_init(void);

// Allocate a physical page (4KB)
// Returns physical address of allocated page, or 0 on failure
uint32_t pmm_alloc_page(void);

// Free a physical page
void pmm_free_page(uint32_t addr);

// Mark a range of physical memory as reserved (unusable)
// Used for kernel, BIOS, memory-mapped I/O regions
void pmm_mark_reserved(uint32_t start, uint32_t end);

// Get total memory managed (in bytes)
uint32_t pmm_get_total_memory(void);

// Get free memory (in bytes)
uint32_t pmm_get_free_memory(void);

#endif /* _PMM_H */