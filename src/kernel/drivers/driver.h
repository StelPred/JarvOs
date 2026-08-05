#ifndef KERNEL_DRIVERS_H
#define KERNEL_DRIVERS_H

#include <stddef.h>
#include <stdint.h>
#include "../include/kernel.h"

/* Device types */
typedef enum {
    DEVICE_TYPE_AUDIO = 0,
    DEVICE_TYPE_INPUT = 1,
    DEVICE_TYPE_DISPLAY = 2,
    DEVICE_TYPE_NETWORK = 3,
    DEVICE_TYPE_STORAGE = 4,
    DEVICE_TYPE_OTHER = 5
} device_type_t;

/* Device operations */
typedef struct {
    int (*init)(void);
    int (*shutdown)(void);
    ssize_t (*read)(void *buf, size_t count, off_t offset);
    ssize_t (*write)(const void *buf, size_t count, off_t offset);
    int (*ioctl)(int request, void *arg);
} device_ops_t;

/* Device structure */
typedef struct device {
    char name[32];
    device_type_t type;
    const device_ops_t *ops;
    void *data; /* Driver-specific data */
    struct device *next;
} device_t;

/* Driver framework functions */
int driver_init(void);
int driver_register(device_t *dev);
int driver_unregister(const char *name);
device_t* driver_lookup(const char *name);
void driver_dump_list(void);

/* Convenience functions for common operations */
ssize_t driver_read(const char *name, void *buf, size_t count, off_t offset);
ssize_t driver_write(const char *name, const void *buf, size_t count, off_t offset);
int driver_ioctl(const char *name, int request, void *arg);

#endif /* KERNEL_DRIVERS_H */