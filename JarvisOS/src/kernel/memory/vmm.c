#include <stdint.h>
#include "vmm.h"
#include "pmm.h"
#include "../hal.h"
#include "../../lib/io.h"

// Page directory and table structures
// We'll use a simple two-level paging (PDE and PTE) for 32-bit x86 with 4KB pages.
// The page directory is an array of 1024 32-bit entries.
// Each page table is an array of 1024 32-bit entries.

// We'll allocate one page directory and a few page tables for the kernel.
// In a real system, we would have multiple page directories (one per process).

// Kernel page directory and table (statically allocated for simplicity)
// We'll place them in the .bss section.

// Page directory for the kernel
static uint32_t kernel_page_directory[1024] __attribute__((aligned(4096)));
// First page table for the kernel (maps the first 4MB of virtual space to physical)
static uint32_t kernel_page_table[2][1024] __attribute__((aligned(4096)));

// Initialize the virtual memory manager
void vmm_init(void) {
    // Initialize the physical memory manager first
    pmm_init();

    // Clear the kernel page directory and table
    for (int i = 0; i < 1024; i++) {
        kernel_page_directory[i] = 0;
    }
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 1024; j++)
            kernel_page_table[i][j] = 0;

    // Set up identity mapping for the first 4MB (virtual 0x00000000 to physical 0x00000000)
    // and also map the kernel's virtual space to physical space.
    // We'll map the kernel's virtual address space (starting at 0xC0000000) to physical memory.
    // This is a common choice: the kernel resides in the upper half of the address space.

    // However, for simplicity, we'll do a direct map of the first 8MB (or whatever we need)
    // to keep the kernel accessible at the same physical and virtual address.

    // We'll map the first 8MB (2 * 4MB) of memory with page tables.

    // We need to set up the page directory entries for the first two page tables (covering 0x00000000 to 0x00800000)
    // and then set up the page table entries.

    // Map the first 8MB: for each page frame, set the corresponding entry in the page table.
    for (uint32_t i = 0; i < 8 * 1024 * 1024 / 0x1000; i++) { // 8MB / 4KB
        uint32_t phys_addr = i * 0x1000;
        // Set the page table entry: present, writable, kernel mode
        kernel_page_table[i / 1024][i % 1024] = phys_addr | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // Now set up the page directory entries for the first two page tables.
    // Note: we only need two page tables to cover 8MB (each page table covers 4MB).
    // But we'll set up the first two entries in the page directory.

    // The address of the page table (must be page aligned)
    uint32_t pt_addr = (uint32_t)kernel_page_table;
    // Set the first two PDEs: present, writable, kernel mode, pointing to the page table
    for (int i = 0; i < 2; i++) {
        kernel_page_directory[i] = pt_addr | PAGE_PRESENT | PAGE_WRITABLE;
        pt_addr += 0x1000; // Each page table is 4KB
    }

    // The rest of the page directory entries are zero (not present).

    // Load the page directory
    vmm_switch_directory(kernel_page_directory);

    // Enable paging
    vmm_enable_paging();
}

// Create a mapping from virtual to physical address
int vmm_map_page(void* virt_addr, uint32_t phys_addr, uint32_t flags) {
    // This is a simplified version that assumes the page table for the given virtual address exists.
    // In a full implementation, we would allocate a page table if it doesn't exist.

    uint32_t vaddr = (uint32_t)virt_addr;
    uint32_t page_dir_index = vaddr >> 22;        // Bits 31-22
    uint32_t page_table_index = (vaddr >> 12) & 0x3FF; // Bits 21-12
    uint32_t offset = vaddr & 0xFFF;              // Bits 11-0

    // Get the page directory entry
    uint32_t* page_dir = vmm_get_current_directory();
    uint32_t pde = page_dir[page_dir_index];

    // Check if the page table for this directory entry exists
    if (!(pde & PAGE_PRESENT)) {
        // We need to allocate a page table
        uint32_t pt_phys = pmm_alloc_page();
        if (pt_phys == 0) {
            return -1; // Out of memory
        }
        // Clear the page table
        uint32_t* pt = (uint32_t*)pt_phys;
        for (int i = 0; i < 1024; i++) {
            pt[i] = 0;
        }
        // Set the page directory entry
        pde = pt_phys | PAGE_PRESENT | PAGE_WRITABLE;
        page_dir[page_dir_index] = pde;
    }

    // Get the page table address
    uint32_t pt_phys = pde & 0xFFFFF000; // Clear the flags
    uint32_t* pt = (uint32_t*)pt_phys;

    // Set the page table entry
    pt[page_table_index] = phys_addr | flags | PAGE_PRESENT | PAGE_WRITABLE;

    // Flush the TLB for this address
    vmm_flush_tlb_entry(virt_addr);

    return 0;
}

// Remove a virtual to physical mapping
void vmm_unmap_page(void* virt_addr) {
    uint32_t vaddr = (uint32_t)virt_addr;
    uint32_t page_dir_index = vaddr >> 22;
    uint32_t page_table_index = (vaddr >> 12) & 0x3FF;

    uint32_t* page_dir = vmm_get_current_directory();
    uint32_t pde = page_dir[page_dir_index];

    if (!(pde & PAGE_PRESENT)) {
        return; // Page table not present
    }

    uint32_t pt_phys = pde & 0xFFFFF000;
    uint32_t* pt = (uint32_t*)pt_phys;

    // Clear the entry
    pt[page_table_index] = 0;

    // Flush the TLB
    vmm_flush_tlb_entry(virt_addr);
}

// Allocate a page and map it at the given virtual address
void* vmm_alloc_map_page(void* virt_addr, uint32_t flags) {
    uint32_t phys_addr = pmm_alloc_page();
    if (phys_addr == 0) {
        return NULL;
    }
    if (vmm_map_page(virt_addr, phys_addr, flags) != 0) {
        pmm_free_page(phys_addr);
        return NULL;
    }
    return virt_addr;
}

// Get the physical address mapped to a virtual address
uint32_t vmm_get_physical_address(void* virt_addr) {
    uint32_t vaddr = (uint32_t)virt_addr;
    uint32_t page_dir_index = vaddr >> 22;
    uint32_t page_table_index = (vaddr >> 12) & 0x3FF;

    uint32_t* page_dir = vmm_get_current_directory();
    uint32_t pde = page_dir[page_dir_index];

    if (!(pde & PAGE_PRESENT)) {
        return 0; // Not mapped
    }

    uint32_t pt_phys = pde & 0xFFFFF000;
    uint32_t* pt = (uint32_t*)pt_phys;

    return pt[page_table_index] & 0xFFFFF000; // Return the physical address (mask out flags)
}

// Switch to a different page directory
void vmm_switch_directory(uint32_t* page_dir) {
    // Load the page directory base register (CR3)
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(page_dir));
}

// Get the current page directory
uint32_t* vmm_get_current_directory(void) {
    uint32_t cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    return (uint32_t*)cr3;
}

// Enable paging
void vmm_enable_paging(void) {
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= PAGE_PRESENT; // Set the paging bit (bit 31)
    __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0));
}

// Disable paging
void vmm_disable_paging(void) {
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~PAGE_PRESENT; // Clear the paging bit
    __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0));
}

// Flush TLB entry for a specific address
void vmm_flush_tlb_entry(void* addr) {
    __asm__ __volatile__("invlpg (%0)" : : "r"(addr) : "memory");
}

// Flush entire TLB
void vmm_flush_tlb(void) {
    // By writing to CR3 again, we flush the TLB
    uint32_t cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(cr3));
}