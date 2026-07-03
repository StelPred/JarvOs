#include "memory.h"
#include "../include/kernel.h"
#include <string.h>

/* Physical memory management */
static page_frame_t page_frames[MAX_PHYSICAL_PAGES];
static uint32_t free_page_count = 0;

/* Kernel heap: we'll use a simple linked list of free blocks */
#define HEAP_START  (PHYSICAL_MEMORY_START + PHYSICAL_MEMORY_SIZE) /* We'll put the heap after the managed physical memory */
#define HEAP_SIZE   (4 * 1024 * 1024)   /* 4 MiB heap for simplicity */

typedef struct heap_block {
    size_t size;          /* Size of the block (including this header) */
    bool free;
    struct heap_block *next;
} heap_block_t;

static heap_block_t *heap_list = NULL;

/* Initialize the memory subsystem */
int mem_init(void) {
    // Initialize physical page frames
    for (uint32_t i = 0; i < MAX_PHYSICAL_PAGES; i++) {
        page_frames[i].addr = PHYSICAL_MEMORY_START + i * PAGE_SIZE;
        page_frames[i].free = true;
    }
    free_page_count = MAX_PHYSICAL_PAGES;

    // Initialize the heap: one big free block
    heap_list = (heap_block_t *)HEAP_START;
    heap_list->size = HEAP_SIZE;
    heap_list->free = true;
    heap_list->next = NULL;

    return KERNEL_SUCCESS;
}

/* Allocate a physical page frame */
void* mem_alloc_page(void) {
    // Simple first-fit
    for (uint32_t i = 0; i < MAX_PHYSICAL_PAGES; i++) {
        if (page_frames[i].free) {
            page_frames[i].free = false;
            free_page_count--;
            return (void*)page_frames[i].addr;
        }
    }
    return NULL; // Out of memory
}

/* Free a physical page frame */
void mem_free_page(void* addr) {
    // Find the page frame by address
    for (uint32_t i = 0; i < MAX_PHYSICAL_PAGES; i++) {
        if (page_frames[i].addr == (uint32_t)addr) {
            page_frames[i].free = true;
            free_page_count++;
            return;
        }
    }
    // If we get here, the address is not in our managed memory
    // In a real kernel, we might panic or ignore
}

/* Allocate memory from the kernel heap */
void* kmalloc(size_t size) {
    // Round up to the nearest pointer size for alignment
    size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    heap_block_t *block = heap_list;
    heap_block_t *prev = NULL;

    while (block != NULL) {
        if (block->free && block->size >= size) {
            // Found a block that fits
            // If the block is much larger than needed, split it
            if (block->size > size + sizeof(heap_block_t)) {
                heap_block_t *new_block = (heap_block_t*)((char*)block + size);
                new_block->size = block->size - size;
                new_block->free = true;
                new_block->next = block->next;

                block->size = size;
                block->next = new_block;
            }
            block->free = false;
            // Return the memory after the header
            return (void*)((char*)block + sizeof(heap_block_t));
        }
        prev = block;
        block = block->next;
    }

    return NULL; // Out of heap memory
}

/* Free memory from the kernel heap */
void kfree(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    // Get the header
    heap_block_t *block = (heap_block_t*)((char*)ptr - sizeof(heap_block_t));

    // Mark the block as free
    block->free = true;

    // Coalesce with the next block if it's free
    if (block->next != NULL && block->next->free) {
        block->size += block->next->size;
        block->next = block->next->next;
    }

    // Note: We could also coalesce with the previous block, but for simplicity we skip it.
    // In a more complete implementation, we would have a doubly linked list or use the boundary tags.
}

/* Get the number of free physical pages */
size_t mem_get_free_pages(void) {
    return free_page_count;
}