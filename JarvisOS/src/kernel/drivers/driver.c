#include "driver.h"
#include "../include/kernel.h"
#include <string.h>

/* Linked list of registered devices */
static device_t *device_list = NULL;

/* Initialize the driver framework */
int driver_init(void) {
    device_list = NULL;
    return KERNEL_SUCCESS;
}

/* Register a device with the framework */
int driver_register(device_t *dev) {
    if (!dev || !dev->name || !dev->ops) {
        return KERNEL_FAILURE;
    }
    /* Check if a device with the same name already exists */
    if (driver_lookup(dev->name) != NULL) {
        return KERNEL_FAILURE; /* Device already registered */
    }
    /* Insert at the head of the list for simplicity */
    dev->next = device_list;
    device_list = dev;
    return KERNEL_SUCCESS;
}

/* Unregister a device by name */
int driver_unregister(const char *name) {
    if (!name) {
        return KERNEL_FAILURE;
    }
    device_t *prev = NULL;
    device_t *curr = device_list;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) {
                prev->next = curr->next;
            } else {
                device_list = curr->next;
            }
            /* We do not free the device structure here because the caller may still need it.
             * In a real kernel, we might have a reference count or let the caller manage it.
             * For now, we just unlink it.
             */
            return KERNEL_SUCCESS;
        }
        prev = curr;
        curr = curr->next;
    }
    return KERNEL_FAILURE; /* Not found */
}

/* Lookup a device by name */
device_t* driver_lookup(const char *name) {
    if (!name) {
        return NULL;
    }
    device_t *curr = device_list;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

/* Dump the list of registered devices (for debugging) */
void driver_dump_list(void) {
    /* In a real kernel, we would print to a console or log.
     * For now, we'll just leave it empty or set a breakpoint.
     */
}

/* Convenience functions */
ssize_t driver_read(const char *name, void *buf, size_t count, off_t offset) {
    device_t *dev = driver_lookup(name);
    if (!dev || !dev->ops->read) {
        return KERNEL_FAILURE;
    }
    return dev->ops->read(buf, count, offset);
}

ssize_t driver_write(const char *name, const void *buf, size_t count, off_t offset) {
    device_t *dev = driver_lookup(name);
    if (!dev || !dev->ops->write) {
        return KERNEL_FAILURE;
    }
    return dev->ops->write(buf, count, offset);
}

int driver_ioctl(const char *name, int request, void *arg) {
    device_t *dev = driver_lookup(name);
    if (!dev || !dev->ops->ioctl) {
        return KERNEL_FAILURE;
    }
    return dev->ops->ioctl(request, arg);
}

/* Example: a null device that does nothing */
static int null_init(void) { return KERNEL_SUCCESS; }
static int null_shutdown(void) { return KERNEL_SUCCESS; }
static ssize_t null_read(void *buf, size_t count, off_t offset) { return 0; }
static ssize_t null_write(const void *buf, size_t count, off_t offset) { return count; }
static int null_ioctl(int request, void *arg) { return KERNEL_FAILURE; /* unsupported */ }

static const device_ops_t null_ops = {
    .init = null_init,
    .shutdown = null_shutdown,
    .read = null_read,
    .write = null_write,
    .ioctl = null_ioctl
};

static device_t null_device = {
    .name = "null",
    .type = DEVICE_TYPE_OTHER,
    .ops = &null_ops,
    .data = NULL,
    .next = NULL
};

/* Function to register the null device (can be called during kernel initialization) */
int driver_register_null_device(void) {
    return driver_register(&null_device);
}