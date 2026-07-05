#include "stdlib.h"
#include "string.h"

#define HEAP_SIZE (1024 * 1024) // 1MB

typedef struct block_header {
    size_t size;
    int free;
    struct block_header* next;
} block_header;

static unsigned char heap[HEAP_SIZE];
static block_header* free_list = NULL;

void* malloc(size_t size) {
    // Align size to 8 bytes for alignment
    size = (size + 7) & ~7;
    if (size == 0) {
        return NULL;
    }

    // Initialize the heap if not done
    if (free_list == NULL) {
        free_list = (block_header*)heap;
        free_list->size = HEAP_SIZE - sizeof(block_header);
        free_list->free = 1;
        free_list->next = NULL;
    }

    // First fit search
    block_header* curr = free_list;
    block_header* prev = NULL;
    while (curr != NULL) {
        if (curr->free && curr->size >= size) {
            // If the block is large enough, split it
            if (curr->size >= size + sizeof(block_header)) {
                // Create a new block for the remaining space
                block_header* new_block = (block_header*)((char*)curr + sizeof(block_header) + size);
                new_block->size = curr->size - size - sizeof(block_header);
                new_block->free = 1;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }
            curr->free = 0;
            return (void*)((char*)curr + sizeof(block_header));
        }
        prev = curr;
        curr = curr->next;
    }

    // No free block found
    return NULL;
}

void free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    block_header* block = (block_header*)((char*)ptr - sizeof(block_header));
    block->free = 1;

    // Coalesce with next block if free
    if (block->next != NULL && block->next->free) {
        block->size += sizeof(block_header) + block->next->size;
        block->next = block->next->next;
    }

    // Note: We are not coalescing with the previous block for simplicity.
    // We could add a doubly linked list or traverse from the beginning, but we skip it for now.
}

void* calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    // Check for overflow
    if (nmemb != 0 && total / nmemb != size) {
        return NULL;
    }
    void* ptr = malloc(total);
    if (ptr != NULL) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    block_header* block = (block_header*)((char*)ptr - sizeof(block_header));
    if (block->size >= size) {
        // The current block is big enough
        // Optionally, we could split if there's a lot of extra space, but we skip for simplicity
        return ptr;
    }

    // Allocate a new block
    void* new_ptr = malloc(size);
    if (new_ptr != NULL) {
        // Copy the old data
        memcpy(new_ptr, ptr, block->size);
        // Free the old block
        free(ptr);
    }
    return new_ptr;
}