# Simple Makefile for JarvisOS with system calls and interrupts
# Assumes you have i686-elf-gcc and nasm

CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy
AR = i686-elf-ar

# Flags
CFLAGS = -ffreestanding -Wall -Wextra -fno-exceptions -m32 -I./src/libc
ASFLAGS = -f bin  # For raw binary output (boot sectors)
LDFLAGS = -T src/kernel/linker.ld --oformat binary

# Libc sources
LIBC_SRCS = \
	src/libc/string.c \
	src/libc/stdlib.c

# Libc object files
LIBC_OBJS = $(LIBC_SRCS:.c=.o)

# Source files
BOOT_ASM = src/boot/boot.asm
BOOT2_ASM = src/boot/boot2.asm
BOOT2_ASM_DEPS = src/boot/gdt.asm

# C source files
KERNEL_C_SRCS = \
	src/kernel/entry.asm \
	src/kernel/main.c \
	src/kernel/core/cpu.c \
	src/kernel/drivers/serial.c \
	src/kernel/drivers/keyboard.c \
	src/kernel/memory/pmm.c \
	src/kernel/memory/vmm.c \
	src/kernel/drivers/pic.c \
	src/kernel/drivers/pit.c \
	src/kernel/drivers/dma.c \
	src/kernel/power.c \
	src/kernel/core/idt.c \
	src/kernel/hal.c \
	src/kernel/syscall.c \
	src/kernel/interrupt/syscall_stub.asm \
	src/kernel/ipc/msg.c \
	src/kernel/core/interrupt.asm

# Object files
KERNEL_OBJS = $(KERNEL_C_SRCS:.c=.o)
KERNEL_OBJS := $(KERNEL_OBJS:.asm=.o)

# Default target - create floppy image
all: floppy.img

# Build libc
libc.a: $(LIBC_OBJS)
	$(AR) rcs $@ $^

# Compile C files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Assemble .asm files to object files
%.o: %.asm
	$(AS) -f elf32 -o $@ $<

# Build stage 1 boot loader (512 bytes)
boot.bin: $(BOOT_ASM)
	$(AS) -f bin -o $@ $<

# Build stage 2 boot loader
boot2.bin: $(BOOT2_ASM) $(BOOT2_ASM_DEPS)
	$(AS) -f bin -o $@ $<

# Build kernel (link with libc for now, though we are freestanding, we can use our libc)
kernel.bin: $(KERNEL_OBJS) libc.a
	$(LD) $(LDFLAGS) -o $@ $^ libc.a

# Create floppy image: boot sector + stage2 + kernel
# We'll put:
# - Sector 1: boot.bin (512 bytes)
# - Sectors 2-3: boot2.bin (should be < 1024 bytes)
# - Sectors 4-?: kernel.bin (the rest)
floppy.img: boot.bin boot2.bin kernel.bin
	# Create empty floppy image (1.44MB = 2880 sectors * 512 bytes)
	dd if=/dev/zero of=floppy.img bs=512 count=2880 2>/dev/null || \
		dd if=/dev/zero of=floppy.img bs=512 count=2880
	# Copy boot sector
	dd if=boot.bin of=floppy.img bs=512 count=1 conv=notrunc
	# Copy stage 2 boot loader
	dd if=boot2.bin of=floppy.img bs=512 seek=1 conv=notrunc
	# Copy kernel
	dd if=kernel.bin of=floppy.img bs=512 seek=3 conv=notrunc

# Run in QEMU
run: floppy.img
	qemu-system-x86_64 -fda floppy.img -d int,cpu_reset -no-reboot -no-shutdown -D qemu.log -monitor stdio

# Clean up
clean:
	rm -f boot.bin boot2.bin kernel.bin libc.a floppy.img
	find src -name '*.o' -delete

.PHONY: all run clean