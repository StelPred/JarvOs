#ifndef _HAL_H
#define _HAL_H

#include <stddef.h>
#include <stdint.h>

// CPU feature flags
#define CPU_FEATURE_FPU      (1 << 0)
#define CPU_FEATURE_VME      (1 << 1)
#define CPU_FEATURE_DE       (1 << 2)
#define CPU_FEATURE_PSE      (1 << 3)
#define CPU_FEATURE_TSC      (1 << 4)
#define CPU_FEATURE_MSR      (1 << 5)
#define CPU_FEATURE_PAE      (1 << 6)
#define CPU_FEATURE_MCE      (1 << 7)
#define CPU_FEATURE_CX8      (1 << 8)
#define CPU_FEATURE_APIC     (1 << 9)
#define CPU_FEATURE_SEP      (1 << 10)
#define CPU_FEATURE_MTRR     (1 << 11)
#define CPU_FEATURE_PGE      (1 << 12)
#define CPU_FEATURE_MCA      (1 << 13)
#define CPU_FEATURE_CMOV     (1 << 14)
#define CPU_FEATURE_PAT      (1 << 15)
#define CPU_FEATURE_PSE36    (1 << 16)
#define CPU_FEATURE_PSN      (1 << 17)
#define CPU_FEATURE_CLFLUSH  (1 << 18)
#define CPU_FEATURE_DTS      (1 << 19)
#define CPU_FEATURE_ACPI     (1 << 20)
#define CPU_FEATURE_MMX      (1 << 21)
#define CPU_FEATURE_FXSR     (1 << 22)
#define CPU_FEATURE_SSE      (1 << 23)
#define CPU_FEATURE_SSE2     (1 << 24)
#define CPU_FEATURE_SS       (1 << 25)
#define CPU_FEATURE_HTT      (1 << 26)
#define CPU_FEATURE_TM       (1 << 27)
#define CPU_FEATURE_IA64     (1 << 28)
#define CPU_FEATURE_PBE      (1 << 29)

// ECX-based features from CPUID leaf 1
#define CPU_FEATURE_SSE3      (1 << 0)
#define CPU_FEATURE_PCLMULQDQ (1 << 1)
#define CPU_FEATURE_DTES64    (1 << 2)
#define CPU_FEATURE_MONITOR   (1 << 3)
#define CPU_FEATURE_DS_CPL    (1 << 4)
#define CPU_FEATURE_VMX       (1 << 5)
#define CPU_FEATURE_SMX       (1 << 6)
#define CPU_FEATURE_EST       (1 << 7)
#define CPU_FEATURE_TM2       (1 << 8)
#define CPU_FEATURE_SSSE3     (1 << 9)
#define CPU_FEATURE_CID       (1 << 10)
#define CPU_FEATURE_FMA       (1 << 11)
#define CPU_FEATURE_CMPXCHG16B (1 << 12)
#define CPU_FEATURE_XTPR      (1 << 13)
#define CPU_FEATURE_PDCM      (1 << 14)
#define CPU_FEATURE_PCID      (1 << 15)
#define CPU_FEATURE_DCA       (1 << 16)
#define CPU_FEATURE_SSE4_1    (1 << 17)
#define CPU_FEATURE_SSE4_2    (1 << 18)
#define CPU_FEATURE_x2APIC    (1 << 19)
#define CPU_FEATURE_MOVBE     (1 << 20)
#define CPU_FEATURE_POPCNT    (1 << 21)
#define CPU_FEATURE_TSC_DEADLINE (1 << 22)
#define CPU_FEATURE_AES       (1 << 23)
#define CPU_FEATURE_XSAVE     (1 << 24)
#define CPU_FEATURE_OSXSAVE   (1 << 25)
#define CPU_FEATURE_AVX       (1 << 26)
#define CPU_FEATURE_F16C      (1 << 27)
#define CPU_FEATURE_RDRAND    (1 << 28)

// Extended CPU features (from CPUID leaf 0x80000001 and higher)
#define CPU_FEATURE_SYSCALL   (1 << 11)   // SYSCALL/SYSRET
#define CPU_FEATURE_LM        (1 << 29)   // Long mode
#define CPU_FEATURE_NX        (1 << 20)   // No-execute page protection
#define CPU_FEATURE_1GBPAGE   (1 << 26)   // 1GB large page
#define CPU_FEATURE_RDTSCP    (1 << 27)   // RDTSCP instruction
#define CPU_FEATURE_EM64T     (1 << 29)   // EMT64 (same as LM)

// Function prototypes for HAL initialization
void hal_init(void);
void hal_cpu_init(void);
void hal_memory_init(void);
void hal_interrupt_init(void);
void hal_timer_init(void);
void hal_serial_init(void);
void hal_keyboard_init(void);
void hal_dma_init(void);
void hal_power_init(void);

// Function to get CPU info (simplified)
uint32_t hal_get_cpu_features(void);
const char* hal_get_cpu_vendor(void);

// Interrupt Descriptor Table initialization
void idt_init(void);

// Function to write to serial port (for debugging)
void hal_serial_write(const char* data, size_t len);

// Memory management functions (HAL)
void* hal_alloc_page(void);
void hal_free_page(void* addr);

// Timer functions (HAL)
void hal_set_timer_frequency(uint32_t hz);
uint64_t hal_get_ticks(void);

// Keyboard functions (HAL)
int hal_keyboard_key_available(void);
uint8_t hal_keyboard_read_scancode(void);

// DMA functions (HAL)
int hal_dma_alloc_channel(void);
int hal_dma_start_transfer(int chan, void* src, void* dst, size_t len, int mode);
int hal_dma_is_complete(int chan);
void hal_dma_free_channel(int chan);

// Power management functions (HAL)
int hal_set_cpu_cstate(uint8_t state);
int hal_set_cpu_pstate(uint16_t mhz);

#endif /* _HAL_H */