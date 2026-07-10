#ifndef KERNEL_FS_VFS_H
#define KERNEL_FS_VFS_H

#include <stddef.h>
#include <stdint.h>
#include "../include/kernel.h"

/* VFS node types */
typedef enum {
    VFS_NODE_TYPE_FILE = 0,
    VFS_NODE_TYPE_DIRECTORY = 1,
    VFS_NODE_TYPE_DEVICE = 2,
    VFS_NODE_TYPE_SYMLINK = 3
} vfs_node_type_t;

/* VFS operations */
typedef struct {
    int (*open)(struct vfs_node *node, int flags);
    int (*close)(struct vfs_node *node);
    ssize_t (*read)(struct vfs_node *node, void *buf, size_t count, off_t offset);
    ssize_t (*write)(struct vfs_node *node, const void *buf, size_t count, off_t offset);
    int (*stat)(struct vfs_node *node, struct stat *st);
} vfs_ops_t;

/* VFS node */
typedef struct vfs_node {
    vfs_node_type_t type;
    char name[64];
    void *data;                  /* Filesystem-specific data */
    const vfs_ops_t *ops;
    struct vfs_node *parent;
    struct vfs_node *child_head; /* First child for directories */
    struct vfs_node *child_tail; /* Last child for directories */
    struct vfs_node *next_sibling;
    struct vfs_node *prev_sibling;
} vfs_node_t;

/* File descriptor */
typedef struct {
    vfs_node_t *node;
    int flags;
    off_t offset;
} fd_t;

/* Standard file descriptors: we'll have a simple table */
#define MAX_OPEN_FILES 64

/* VFS functions */
int vfs_init(void);
int vfs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long flags, const void *data);
vfs_node_t *vfs_lookup(const char *path);
int vfs_open(const char *path, int flags);
int vfs_close(int fd);
ssize_t vfs_read(int fd, void *buf, size_t count);
ssize_t vfs_write(int fd, const void *buf, size_t count);
int vfs_stat(const char *path, struct stat *st);

/* For simplicity, we'll define a few flags */
#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR   0x0002
#define O_CREAT  0x0040
#define O_TRUNC  0x0200
#define O_APPEND 0x0400

#endif /* KERNEL_FS_VFS_H */