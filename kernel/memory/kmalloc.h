#ifndef _KMALLOC_H
#define _KMALLOC_H
#include <stddef.h>

void kmalloc_init(void);
void* kmalloc(size_t size);
void* krealloc(void *ptr, size_t size);
void kfree(void *ptr);

#endif /* _KMALLOC_H */