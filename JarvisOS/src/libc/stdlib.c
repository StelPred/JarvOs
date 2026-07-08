#include "stdlib.h"
#include "../kernel/memory/memory.h"
#include <string.h>

void* malloc(size_t size) {
    return kmalloc(size);
}

void free(void* ptr) {
    kfree(ptr);
}

void* calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    if (nmemb != 0 && total / nmemb != size) {
        return NULL; // overflow check
    }
    void* ptr = kmalloc(total);
    if (ptr != NULL) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return kmalloc(size);
    }
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }
    void* new_ptr = kmalloc(size);
    if (new_ptr != NULL) {
        memcpy(new_ptr, ptr, size); // may over-copy, acceptable for now
        kfree(ptr);
    }
    return new_ptr;
}