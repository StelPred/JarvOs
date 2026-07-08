#include "dma.h"
#include "../../lib/io.h"

// Global flag to track if DMA controller has been initialized
static int dma_initialized = 0;

// Initialize the DMA controller
void dma_init(void) {
    // Reset the DMA controller
    outb(DMA1_MASTER_CLEAR, 0);
    io_wait();

    // Clear the flip-flop
    outb(DMA1_CLEAR_BYTE_FLIP, 0);
    io_wait();

    // Set all channels to demand mode, single transfer, write (we'll change as needed)
    // Mask all channels for now
    outb(DMA1_WRITE_MASK, 0x0F);
    io_wait();

    dma_initialized = 1;
}

// Allocate a DMA channel
int dma_alloc_channel(void) {
    if (!dma_initialized) {
        dma_init();
    }

    // Simple allocation: find first unmasked channel
    // In a real implementation, we would track which channels are allocated
    // For now, we'll just return channel 0 and assume the caller manages it properly
    // This is a simplification for the demo
    return 0;
}

// Free a DMA channel
void dma_free_channel(int channel) {
    // In a simple implementation, we just mask the channel
    if (channel < 0 || channel > 3) {
        return;
    }
    uint8_t mask = inb(DMA1_MASK);
    mask |= (1 << channel);
    outb(DMA1_MASK, mask);
}

// Start a DMA transfer
int dma_start_transfer(int channel, void* src, void* dst, size_t len, uint16_t mode) {
    if (!dma_initialized) {
        dma_init();
    }

    if (channel < 0 || channel > 3) {
        return -1;
    }

    if (len == 0 || len > 65536) {
        return -1;
    }

    uint16_t addr_port, count_port;
    uint8_t  mask;

    // Determine the port addresses for this channel
    switch (channel) {
        case 0:
            addr_port = DMA1_CHANNEL0_ADDR;
            count_port = DMA1_CHANNEL0_COUNT;
            mask = 0x01;
            break;
        case 1:
            addr_port = DMA1_CHANNEL1_ADDR;
            count_port = DMA1_CHANNEL1_COUNT;
            mask = 0x02;
            break;
        case 2:
            addr_port = DMA1_CHANNEL2_ADDR;
            count_port = DMA1_CHANNEL2_COUNT;
            mask = 0x04;
            break;
        case 3:
            addr_port = DMA1_CHANNEL3_ADDR;
            count_port = DMA1_CHANNEL3_COUNT;
            mask = 0x08;
            break;
        default:
            return -1;
    }

    // Mask the channel while we set it up
    uint8_t old_mask = inb(DMA1_MASK);
    outb(DMA1_MASK, old_mask | mask);
    io_wait();

    // Clear the flip-flop
    outb(DMA1_CLEAR_BYTE_FLIP, 0);
    io_wait();

    // Set the mode
    outb(DMA1_MODE, mode | channel);
    io_wait();

    // Set the address (low byte then high byte)
    uint32_t src_addr = (uint32_t)src;
    outb(addr_port, src_addr & 0xFF);
    io_wait();
    outb(addr_port, (src_addr >> 8) & 0xFF);
    io_wait();

    // For simplicity, we're doing memory-to-memory transfer?
    // Actually, in DMA, one side is usually I/O and the other is memory.
    // But for this example, we'll assume we're setting up a memory-to-memory transfer
    // by programming both address and count appropriately.
    // In reality, the DMA controller has separate address and count registers for source and destination
    // only in certain modes, but the 8237 has a single address counter that increments or decrements.
    // For memory-to-memory, we'd need to use two channels, but that's complex.

    // Let's simplify: we'll just set up for a memory read (from src to I/O port) or memory write (from I/O port to dst)
    // But since we don't have a specific I/O device in mind, we'll just show how to set up the address.

    // Actually, the 8237 DMA controller in the PC is typically used for:
    // - Channel 0: DRAM refresh
    // - Channel 1: Not assigned
    // - Channel 2: Floppy disk
    // - Channel 3: Not assigned
    // So for a general purpose DMA, we might need to use it in a non-standard way or just acknowledge
    // that it's primarily for specific devices.

    // For the sake of this example, we'll program it for a memory-to-memory transfer by treating
    // the "destination" as the memory location and assuming the source is coming from an
    // external device that we're not simulating. This isn't realistic but shows the register setup.

    // Set the low and high bytes of the address
    // We already set the address above - but we need to decide if it's source or destination
    // In the 8237, the address register holds the memory address for the transfer.
    // The I/O address is implied by the channel (for fixed I/O devices) or set elsewhere.
    // For simplicity, we'll just set the memory address as we did above.

    // Set the transfer count (low byte then high byte)
    // Note: the count is number of transfers - 1
    uint16_t count = len - 1;
    outb(count_port, count & 0xFF);
    io_wait();
    outb(count_port, (count >> 8) & 0xFF);
    io_wait();

    // Set the mode register again to ensure it's set
    outb(DMA1_MODE, mode | channel);
    io_wait();

    // Unmask the channel to start the transfer
    outb(DMA1_MASK, old_mask & ~mask);
    io_wait();

    return 0;
}

// Check if DMA transfer is complete
int dma_is_complete(int channel) {
    if (!dma_initialized) {
        return -1;
    }

    if (channel < 0 || channel > 3) {
        return -1;
    }

    uint8_t status = inb(DMA1_STATUS);
    // Check if the terminal count bit for this channel is set
    return (status & (1 << (channel + 4))) != 0;
}

// Get DMA controller status
uint8_t dma_get_status(void) {
    if (!dma_initialized) {
        dma_init();
    }
    return inb(DMA1_STATUS);
}