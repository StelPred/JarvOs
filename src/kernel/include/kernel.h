#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>

/* Basic types and definitions for the kernel */

/* Return types */
typedef int bool;
#define true 1
#define false 0

/* Error codes */
#define KERNEL_SUCCESS 0
#define KERNEL_FAILURE -1

/* Forward declarations for subsystems */
struct process;
struct memory_context;
struct vfs_node;
struct device_driver;

/* Function prototypes for subsystem initialization */
int kernel_init_process_subsystem(void);
int kernel_init_memory_subsystem(void);
int kernel_init_vfs_subsystem(void);
int kernel_init_drivers_subsystem(void);

#endif /* KERNEL_H */