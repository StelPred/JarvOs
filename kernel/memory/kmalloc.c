#include "kmalloc.h"
#include "vmm.h"
#include <stdint.h>
#include <string.h>

#define KHEAP_START   0xD0000000
#define KMALLOC_PAGE  0x1000

/* Every allocation is preceded by this header. 'next'/'prev' link
   blocks in memory order (not free-list order), so we can merge
   a freed block with its physical neighbors. */
typedef struct block_header {
    size_t size;                 /* size of usable data area */
    int free;
    struct block_header *next;   /* next block in memory */
    struct block_header *prev;   /* previous block in memory */
} block_header_t;

static block_header_t *heap_head = NULL;
static uint32_t heap_end = KHEAP_START;

void kmalloc_init(void) {
    heap_head = NULL;
    heap_end = KHEAP_START;
}

/* Map fresh pages at the end of the heap and turn them into one free block */
static block_header_t* expand_heap(size_t min_size) {
    size_t total = min_size + sizeof(block_header_t);
    size_t pages = (total + KMALLOC_PAGE - 1) / KMALLOC_PAGE;

    uint32_t block_addr = heap_end;
    for (size_t i = 0; i < pages; i++) {
        void *vaddr = (void*)(heap_end + i * KMALLOC_PAGE);
        if (vmm_alloc_map_page(vaddr, PAGE_WRITABLE) == NULL) {
            return NULL; /* out of physical memory */
        }
    }
    heap_end += pages * KMALLOC_PAGE;

    block_header_t *block = (block_header_t*)block_addr;
    block->size = pages * KMALLOC_PAGE - sizeof(block_header_t);
    block->free = 1;
    block->next = NULL;
    block->prev = NULL;
    return block;
}

/* Split a block if there's enough leftover space to form a new free block */
static void split_block(block_header_t *block, size_t size) {
    size_t remaining = block->size - size;
    if (remaining <= sizeof(block_header_t) + 16) {
        return; /* not worth splitting */
    }
    block_header_t *new_block = (block_header_t*)((uint8_t*)(block + 1) + size);
    new_block->size = remaining - sizeof(block_header_t);
    new_block->free = 1;
    new_block->next = block->next;
    new_block->prev = block;
    if (block->next) block->next->prev = new_block;
    block->next = new_block;
    block->size = size;
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;
    size = (size + 7) & ~((size_t)7); /* 8-byte align */

    block_header_t *block = heap_head;
    block_header_t *last = NULL;
    while (block) {
        if (block->free && block->size >= size) {
            split_block(block, size);
            block->free = 0;
            return (void*)(block + 1);
        }
        last = block;
        block = block->next;
    }

    /* No fitting free block: grow the heap */
    block_header_t *fresh = expand_heap(size);
    if (!fresh) return NULL;

    if (last) {
        last->next = fresh;
        fresh->prev = last;
    } else {
        heap_head = fresh;
    }

    split_block(fresh, size);
    fresh->free = 0;
    return (void*)(fresh + 1);
}

void kfree(void *ptr) {
    if (!ptr) return;
    block_header_t *block = (block_header_t*)ptr - 1;
    block->free = 1;

    /* Merge with next block if it's free */
    if (block->next && block->next->free) {
        block_header_t *next = block->next;
        block->size += sizeof(block_header_t) + next->size;
        block->next = next->next;
        if (next->next) next->next->prev = block;
    }
    /* Merge with previous block if it's free */
    if (block->prev && block->prev->free) {
        block_header_t *prev = block->prev;
        prev->size += sizeof(block_header_t) + block->size;
        prev->next = block->next;
        if (block->next) block->next->prev = prev;
    }
}

void* krealloc(void *ptr, size_t size) {
    if (!ptr) return kmalloc(size);
    if (size == 0) { kfree(ptr); return NULL; }

    block_header_t *block = (block_header_t*)ptr - 1;
    if (block->size >= size) {
        return ptr; /* current block already big enough */
    }
    void *new_ptr = kmalloc(size);
    if (!new_ptr) return NULL;
    memcpy(new_ptr, ptr, block->size);
    kfree(ptr);
    return new_ptr;
}