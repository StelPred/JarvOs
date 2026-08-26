#ifndef _TSS_H
#define _TSS_H

#include <stdint.h>

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10
#define USER_CS   0x1B   // 0x18 | ring 3
#define USER_DS   0x23   // 0x20 | ring 3
#define TSS_SEL   0x28

void tss_init(void);
void tss_set_kernel_stack(uint32_t esp0);

#endif /* _TSS_H */