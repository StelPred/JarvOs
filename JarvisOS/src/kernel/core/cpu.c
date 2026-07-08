#include <stdint.h>
#include "../hal.h"
#include "../lib/io.h"

// CPUID function
static void cpuid(uint32_t op, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    __asm__ __volatile__(
        "cpuid"
        : "=a"(*eax),
          "=b"(*ebx),
          "=c"(*ecx),
          "=d"(*edx)
        : "0"(op)
    );
}

// Extended CPUID for higher leafs
static void cpuid_ex(uint32_t op, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    __asm__ __volatile__(
        "cpuid"
        : "=a"(*eax),
          "=b"(*ebx),
          "=c"(*ecx),
          "=d"(*edx)
        : "0"(op)
    );
}

// Get CPU vendor string
const char* hal_get_cpu_vendor(void) {
    static char vendor[13]; // 12 characters + null terminator
    uint32_t eax, ebx, ecx, edx;

    cpuid(0, &eax, &ebx, &ecx, &edx);
    // The vendor string is in ebx, edx, ecx (in that order)
    *(uint32_t*)(vendor+0) = ebx;
    *(uint32_t*)(vendor+4) = edx;
    *(uint32_t*)(vendor+8) = ecx;
    vendor[12] = '\0';

    return vendor;
}

// Get CPU features (from CPUID leaf 1 and extended leafs)
uint32_t hal_get_cpu_features(void) {
    uint32_t eax, ebx, ecx, edx;
    uint32_t features = 0;

    // Get basic features
    cpuid(1, &eax, &ebx, &ecx, &edx);

    if (edx & (1 << 0))  features |= CPU_FEATURE_FPU;
    if (edx & (1 << 1))  features |= CPU_FEATURE_VME;
    if (edx & (1 << 2))  features |= CPU_FEATURE_DE;
    if (edx & (1 << 3))  features |= CPU_FEATURE_PSE;
    if (edx & (1 << 4))  features |= CPU_FEATURE_TSC;
    if (edx & (1 << 5))  features |= CPU_FEATURE_MSR;
    if (edx & (1 << 6))  features |= CPU_FEATURE_PAE;
    if (edx & (1 << 7))  features |= CPU_FEATURE_MCE;
    if (edx & (1 << 8))  features |= CPU_FEATURE_CX8;
    if (edx & (1 << 9))  features |= CPU_FEATURE_APIC;
    if (edx & (1 << 10)) features |= CPU_FEATURE_SEP;
    if (edx & (1 << 11)) features |= CPU_FEATURE_MTRR;
    if (edx & (1 << 12)) features |= CPU_FEATURE_PGE;
    if (edx & (1 << 13)) features |= CPU_FEATURE_MCA;
    if (edx & (1 << 14)) features |= CPU_FEATURE_CMOV;
    if (edx & (1 << 15)) features |= CPU_FEATURE_PAT;
    if (edx & (1 << 16)) features |= CPU_FEATURE_PSE36;
    if (edx & (1 << 17)) features |= CPU_FEATURE_PSN;
    if (edx & (1 << 18)) features |= CPU_FEATURE_CLFLUSH;
    if (edx & (1 << 19)) features |= CPU_FEATURE_DTS;
    if (edx & (1 << 21)) features |= CPU_FEATURE_MMX;
    if (edx & (1 << 22)) features |= CPU_FEATURE_FXSR;
    if (edx & (1 << 24)) features |= CPU_FEATURE_SSE2;
    if (edx & (1 << 25)) features |= CPU_FEATURE_SS;
    if (edx & (1 << 26)) features |= CPU_FEATURE_HTT;
    if (edx & (1 << 27)) features |= CPU_FEATURE_TM;
    if (edx & (1 << 29)) features |= CPU_FEATURE_PBE;

    if (ecx & (1 << 0))  features |= CPU_FEATURE_SSE3;
    if (ecx & (1 << 1))  features |= CPU_FEATURE_PCLMULQDQ;
    if (ecx & (1 << 2))  features |= CPU_FEATURE_DTES64;
    if (ecx & (1 << 3))  features |= CPU_FEATURE_MONITOR;
    if (ecx & (1 << 4))  features |= CPU_FEATURE_DS_CPL;
    if (ecx & (1 << 5))  features |= CPU_FEATURE_VMX;
    if (ecx & (1 << 6))  features |= CPU_FEATURE_SMX;
    if (ecx & (1 << 7))  features |= CPU_FEATURE_EST;
    if (ecx & (1 << 8))  features |= CPU_FEATURE_TM2;
    if (ecx & (1 << 9))  features |= CPU_FEATURE_SSSE3;
    if (ecx & (1 << 10)) features |= CPU_FEATURE_CID;
    if (ecx & (1 << 11)) features |= CPU_FEATURE_FMA;
    if (ecx & (1 << 12)) features |= CPU_FEATURE_CMPXCHG16B;
    if (ecx & (1 << 13)) features |= CPU_FEATURE_XTPR;
    if (ecx & (1 << 14)) features |= CPU_FEATURE_PDCM;
    if (ecx & (1 << 15)) features |= CPU_FEATURE_PCID;
    if (ecx & (1 << 15)) features |= CPU_FEATURE_DCA;
    if (ecx & (1 << 16)) features |= CPU_FEATURE_SSE4_1;
    if (ecx & (1 << 17)) features |= CPU_FEATURE_SSE4_2;
    if (ecx & (1 << 18)) features |= CPU_FEATURE_x2APIC;
    if (ecx & (1 << 19)) features |= CPU_FEATURE_MOVBE;
    if (ecx & (1 << 20)) features |= CPU_FEATURE_POPCNT;
    if (ecx & (1 << 21)) features |= CPU_FEATURE_TSC_DEADLINE;
    if (ecx & (1 << 22)) features |= CPU_FEATURE_AES;
    if (ecx & (1 << 23)) features |= CPU_FEATURE_XSAVE;
    if (ecx & (1 << 24)) features |= CPU_FEATURE_OSXSAVE;
    if (ecx & (1 << 25)) features |= CPU_FEATURE_AVX;
    if (ecx & (1 << 26)) features |= CPU_FEATURE_F16C;
    if (ecx & (1 << 27)) features |= CPU_FEATURE_RDRAND;

    // Check for SYSCALL/SYSRET (in EDX for AMD, but we'll check both)
    if (edx & (1 << 11)) features |= CPU_FEATURE_SYSCALL; // Actually, SYSCALL is in EDX bit 11 for AMD, but let's note it's not standard in Intel until later? We'll put it here for now.

    // We'll also check for long mode (from extended CPUID)
    cpuid_ex(0x80000000, &eax, &ebx, &ecx, &edx);
    if (eax >= 0x80000001) {
        cpuid_ex(0x80000001, &eax, &ebx, &ecx, &edx);
        if (edx & (1 << 29)) features |= CPU_FEATURE_LM; // Long mode
        if (edx & (1 << 28)) features |= CPU_FEATURE_SYSCALL; // SYSCALL for AMD in long mode?
        if (edx & (1 << 26)) features |= CPU_FEATURE_PAE; // Already set from CPUID 1, but double-check
        if (edx & (1 << 20)) features |= CPU_FEATURE_NX; // No-execute
    }

    return features;
}

// Initialize CPU (enable FPU, SSE, etc.)
void hal_cpu_init(void) {
    // Enable FPU and SSE in CR0 and CR4
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x00000002; // MP (Monitor Coprocessor) - but we want to set the EM bit? Actually, we want to clear EM and TS to use FPU.
    // We want to set the NE bit (bit 5) for x87 exceptions to be interrupt-driven, and clear EM and TS.
    // Let's set up CR0 properly:
    //   PE (bit 0) already set by bootloader
    //   MP (bit 1) = 1 (we want to wait for FPU)
    //   EM (bit 2) = 0 (we want to use FPU, not emulate)
    //   TS (bit 3) = 0 (we want to allow FPU use)
    //   NE (bit 5) = 1 (we want x87 exceptions to be interrupt-driven)
    //   WP (bit 16) = 1 (write protect)
    //   AM (bit 18) = 1 (alignment mask)
    //   NW (bit 29) = 0 (not write-through)
    //   CD (bit 30) = 0 (cache disabled)
    //   PG (bit 31) = 0 (paging) - we'll set this later in the MMU init
    cr0 = (cr0 & 0x80000001) | 0x00000012; // Keep PG and PE, set MP, clear EM, TS, set NE
    __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0));

    // Enable SSE in CR4
    uint32_t cr4;
    __asm__ __volatile__("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x00000600; // OSFXSR (bit 9) and OSXMMEXCPT (bit 10) for SSE exceptions
    __asm__ __volatile__("mov %0, %%cr4" : : "r"(cr4));

    // Initialize x87 FPU
    __asm__ __volatile__("fninit");
}

// Initialize the HAL (called from kernel_main)
// void hal_init(void) {
//     hal_cpu_init();
//     // We'll add other init functions as we implement them
//     // hal_memory_init();
//     // hal_interrupt_init();
//     // hal_timer_init();
// }