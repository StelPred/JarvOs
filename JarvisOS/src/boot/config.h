#ifndef _BOOT_CONFIG_H
#define _BOOT_CONFIG_H

/* Bootloader configuration options */

/* Kernel load address (physical address where kernel will be loaded) */
#define KERNEL_LOAD_ADDR    0x00100000  /* 1MB */

/* Boot device options */
#define BOOT_DEVICE_FLOPPY  0
#define BOOT_DEVICE_HD      0x80
#define BOOT_DEVICE_CD      0xE0
#define DEFAULT_BOOT_DEVICE BOOT_DEVICE_FLOPPY

/* Debug output options */
#define DEBUG_OUTPUT_SERIAL 1
#define DEBUG_OUTPUT_VGA    2
#define DEFAULT_DEBUG_OUTPUT DEBUG_OUTPUT_SERIAL

/* Serial port configuration */
#define SERIAL_PORT_COM1    0x3F8
#define SERIAL_PORT_COM2    0x2F8
#define SERIAL_PORT_COM3    0x3E8
#define SERIAL_PORT_COM4    0x2E8
#define DEFAULT_SERIAL_PORT SERIAL_PORT_COM1

/* Serial baud rate */
#define SERIAL_BAUD_RATE    9600

/* VGA text mode configuration */
#define VGA_MEMORY          0xB8000
#define VGA_WIDTH           80
#define VGA_HEIGHT          25

/* Framebuffer options (if available) */
#define FRAMEBUFFER_WIDTH   800
#define FRAMEBUFFER_HEIGHT  600
#define FRAMEBUFFER_DEPTH   32

/* Enable/disable features */
#define ENABLE_MULTIBOOT2   1
#define ENABLE_DEBUG_OUTPUT 1
#define ENABLE_SERIAL_DEBUG 1
#define ENABLE_VGA_DEBUG    0

#endif /* _BOOT_CONFIG_H */