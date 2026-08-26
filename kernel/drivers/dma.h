#ifndef _DMA_H
#define _DMA_H

#include <stdint.h>
#include <stddef.h>

// DMA (Direct Memory Access) controller for 8237

// I/O ports for the DMA controller
#define DMA1_CHANNEL0_ADDR  0x00
#define DMA1_CHANNEL0_COUNT 0x01
#define DMA1_CHANNEL1_ADDR  0x02
#define DMA1_CHANNEL1_COUNT 0x03
#define DMA1_CHANNEL2_ADDR  0x04
#define DMA1_CHANNEL2_COUNT 0x05
#define DMA1_CHANNEL3_ADDR  0x06
#define DMA1_CHANNEL3_COUNT 0x07
#define DMA1_STATUS         0x08
#define DMA1_COMMAND        0x08
#define DMA1_REQUEST        0x09
#define DMA1_MASK           0x0A
#define DMA1_MODE           0x0B
#define DMA1_CLEAR_BYTE_FLIP 0x0C
#define DMA1_MASTER_CLEAR   0x0D
#define DMA1_CLEAR_MASK     0x0E
#define DMA1_WRITE_MASK     0x0F

// DMA modes
#define DMA_MODE_DEMAND     0x00
#define DMA_MODE_SINGLE     0x20
#define DMA_MODE_BLOCK      0x40
#define DMA_MODE_CASCADE    0x60

// Transfer types
#define DMA_TRANSFER_READ   0x44  // Read from memory
#define DMA_TRANSFER_WRITE  0x48  // Write to memory
#define DMA_TRANSFER_VERIFY 0x80  // Verify transfer

// Initialize the DMA controller
void dma_init(void);

// Allocate a DMA channel
int dma_alloc_channel(void);

// Free a DMA channel
void dma_free_channel(int channel);

// Start a DMA transfer
// src: source address (physical)
// dst: destination address (physical)
// len: transfer length in bytes
// mode: transfer mode (see DMA_MODE_* and DMA_TRANSFER_*)
// Returns 0 on success, -1 on failure
int dma_start_transfer(int channel, void* src, void* dst, size_t len, uint16_t mode);

// Check if DMA transfer is complete
int dma_is_complete(int channel);

// Get DMA controller status
uint8_t dma_get_status(void);

#endif /* _DMA_H */