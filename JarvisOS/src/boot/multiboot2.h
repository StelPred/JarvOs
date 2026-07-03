#ifndef _MULTIBOOT2_H
#define _MULTIBOOT2_H

/* Multiboot2 header magic numbers */
#define MULTIBOOT2_HEADER_MAGIC         0xE85250D6
#define MULTIBOOT2_HEADER_TAG_END       0
#define MULTIBOOT2_HEADER_TAG_CMDLINE   1
#define MULTIBOOT2_HEADER_TAG_ADDR      4
#define MULTIBOOT2_HEADER_TAG_ENTRY_ADDR 9

/* Multiboot2 header tag structure */
struct multiboot_header_tag {
    uint32_t type;
    uint32_t size;
} __attribute__((packed));

/* Multiboot2 header */
struct multiboot_header {
    uint32_t magic;
    uint32_t architecture;
    uint32_t header_length;
    uint32_t checksum;
} __attribute__((packed));

/* Multiboot2 header tag for address fields */
struct multiboot_header_addr {
    struct multiboot_header_tag tag;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} __attribute__((packed));

/* Multiboot2 header tag for entry address */
struct multiboot_header_entry_addr {
    struct multiboot_header_tag tag;
    uint32_t entry_addr;
} __attribute__((packed));

/* Multiboot2 header tag for cmdline */
struct multiboot_header_tag_str {
    struct multiboot_header_tag tag;
    uint8_t string[0];
} __attribute__((packed));

#endif /* _MULTIBOOT2_H */