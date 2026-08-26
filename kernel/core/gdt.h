#ifndef _GDT_H
#define _GDT_H
#include <stdint.h>
void gdt_init(void);
uint64_t* gdt_get_table(void);
#endif /* _GDT_H */