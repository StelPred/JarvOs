#ifndef _VMM_H
#define _VMM_H

#include <stdint.h>
#include <stddef.h>

// Virtual Memory Manager
// Manages virtual address space and paging

// Page directory and table entry flags
#define PAGE_PRESENT    0x001  // Page is present in memory
#define PAGE_WRITABLE   0x002  // Page is writable
#define PAGE_USER       0x004  // Accessible from user mode
#define PAGE_WRITE_THRU 0x010  // Write-through caching
#define PAGE_CACHE_DIS  0x020  // Cache disabled
#define PAGE_ACCESSED   0x040  // Page has been accessed
#define PAGE_DIRTY      0x080  // Page has been written to
#define PAGE_SIZE_4MB   0x080  // 4MB page (PDE only)
#define PAGE_GLOBAL     0x100  // Global page (ignored if PGE not set in CR4)

// Initialize the virtual memory manager
void vmm_init(void);

// Create a mapping from virtual to physical address
// Returns 0 on success, negative on failure
int vmm_map_page(void* virt_addr, uint32_t phys_addr, uint32_t flags);

// Remove a virtual to physical mapping
void vmm_unmap_page(void* virt_addr);

// Allocate a page and map it at the given virtual address
void* vmm_alloc_map_page(void* virt_addr, uint32_t flags);

// Get the physical address mapped to a virtual address
uint32_t vmm_get_physical_address(void* virt_addr);

// Switch to a different page directory
void vmm_switch_directory(uint32_t* page_dir);

// Get the current page directory
uint32_t* vmm_get_current_directory(void);
uint32_t* vmm_create_address_space(void);

// Enable paging
void vmm_enable_paging(void);

// Disable paging
void vmm_disable_paging(void);

// Flush TLB entry for a specific address
void vmm_flush_tlb_entry(void* addr);

// Flush entire TLB
void vmm_flush_tlb(void);

#endif /* _VMM_H */