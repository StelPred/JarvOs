#include "vfs.h"
#include "../include/kernel.h"
#include <string.h>

#define MAX_PATH 256

/* We'll implement a simple in-memory filesystem (ramfs) for the VFS */

/* Forward declaration of the ramfs filesystem type */
static const vfs_ops_t ramfs_ops;

/* Root directory of the VFS */
static vfs_node_t *vfs_root = NULL;

/* Current working directory (we'll keep it simple and not support multiple processes for now) */
static vfs_node_t *vfs_cwd = NULL;

/* File descriptor table */
static fd_t fd_table[MAX_OPEN_FILES];
static int next_fd = 3; /* Start after stdin/stdout/stderr, but we don't have those yet */

/* Helper function to duplicate a string in kernel memory */
static char* kstrdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = (char*)kmalloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}

/* Helper function to create a new vfs node */
static vfs_node_t* vfs_create_node(const char *name, vfs_node_type_t type, vfs_node_t *parent) {
    vfs_node_t *node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    if (!node) {
        return NULL;
    }
    memset(node, 0, sizeof(vfs_node_t));
    strncpy(node->name, name, sizeof(node->name)-1);
    node->name[sizeof(node->name)-1] = '\0';
    node->type = type;
    node->parent = parent;
    node->ops = &ramfs_ops; /* For now, all nodes use the ramfs ops */
    node->data = NULL; /* We'll use data for file content and directory children */
    return node;
}

/* Helper function to add a child to a directory */
static int vfs_dir_add_child(vfs_node_t *dir, vfs_node_t *child) {
    if (dir->type != VFS_NODE_TYPE_DIRECTORY) {
        return KERNEL_FAILURE;
    }
    child->parent = dir;
    if (dir->child_head == NULL) {
        dir->child_head = dir->child_tail = child;
        child->next_sibling = child->prev_sibling = NULL;
    } else {
        dir->child_tail->next_sibling = child;
        child->prev_sibling = dir->child_tail;
        child->next_sibling = NULL;
        dir->child_tail = child;
    }
    return KERNEL_SUCCESS;
}

/* Helper function to find a child by name in a directory */
static vfs_node_t* vfs_dir_find_child(vfs_node_t *dir, const char *name) {
    if (dir->type != VFS_NODE_TYPE_DIRECTORY) {
        return NULL;
    }
    for (vfs_node_t *child = dir->child_head; child != NULL; child = child->next_sibling) {
        if (strcmp(child->name, name) == 0) {
            return child;
        }
    }
    return NULL;
}

/* Helper function to remove a child from a directory */
static void vfs_dir_remove_child(vfs_node_t *dir, vfs_node_t *child) {
    if (dir->type != VFS_NODE_TYPE_DIRECTORY) {
        return;
    }
    if (child->prev_sibling) {
        child->prev_sibling->next_sibling = child->next_sibling;
    } else {
        dir->child_head = child->next_sibling;
    }
    if (child->next_sibling) {
        child->next_sibling->prev_sibling = child->prev_sibling;
    } else {
        dir->child_tail = child->prev_sibling;
    }
    child->parent = NULL;
    child->next_sibling = child->prev_sibling = NULL;
}

/* Simple stat structure for our VFS */
struct stat {
    uint32_t st_mode;   /* File type and mode */
    uint32_t st_size;   /* Total size, in bytes */
    uint32_t st_blocks; /* Number of 512B blocks allocated */
};

/* Macros for st_mode */
#define S_IFMT  0xF000         /* Type of file */
#define S_IFDIR  0x4000        /* Directory */
#define S_IFCHR  0x2000        /* Character device */
#define S_IFBLK  0x6000        /* Block device */
#define S_IFREG  0x8000        /* Regular file */
#define S_IFIFO  0x1000        /* FIFO */
#define S_IFLNK  0xA000        /* Symbolic link */
#define S_IFSOCK 0xC000        /* Socket */
#define S_IRWXU  0000700       /* RWX mask for owner */
#define S_IRUSR  0000400       /* R for owner */
#define S_IWUSR  0000200       /* W for owner */
#define S_IXUSR  0000100       /* X for owner */
#define S_IRWXG  0000070       /* RWX mask for group */
#define S_IRGRP  0000040       /* R for group */
#define S_IWGRP  0000020       /* W for group */
#define S_IXGRP  0000010       /* X for group */
#define S_IRWXO  0000007       /* RWX mask for others */
#define S_IROTH  0000004       /* R for others */
#define S_IWOTH  0000002       /* W for others */
#define S_IXOTH  0000001       /* X for others */

/* Ramfs specific data for a file: we'll store the content and size */
typedef struct {
    char *content;
    size_t size;
    size_t capacity;
} ramfs_file_data_t;

/* Ramfs specific data for a directory: we don't need extra data beyond the VFS node's child list */
typedef struct {
    /* Nothing needed */
} ramfs_dir_data_t;

/* Ramfs node data union */
typedef union {
    ramfs_file_data_t file;
    ramfs_dir_data_t dir;
} ramfs_data_t;

/* Ramfs operations */

static int ramfs_open(vfs_node_t *node, int flags) {
    /* For simplicity, we ignore flags and just allow read/write */
    /* In a real implementation, we would check the flags and set up an fd accordingly */
    return KERNEL_SUCCESS;
}

static int ramfs_close(vfs_node_t *node) {
    return KERNEL_SUCCESS;
}

static ssize_t ramfs_read(vfs_node_t *node, void *buf, size_t count, off_t offset) {
    if (node->type != VFS_NODE_TYPE_FILE) {
        return KERNEL_FAILURE;
    }
    ramfs_file_data_t *data = (ramfs_file_data_t*)node->data;
    if (!data) {
        return 0;
    }
    if (offset >= data->size) {
        return 0; /* EOF */
    }
    size_t bytes_to_read = count;
    if (offset + bytes_to_read > data->size) {
        bytes_to_read = data->size - offset;
    }
    memcpy(buf, data->content + offset, bytes_to_read);
    return bytes_to_read;
}

static ssize_t ramfs_write(vfs_node_t *node, const void *buf, size_t count, off_t offset) {
    if (node->type != VFS_NODE_TYPE_FILE) {
        return KERNEL_FAILURE;
    }
    ramfs_file_data_t *data = (ramfs_file_data_t*)node->data;
    if (!data) {
        /* Allocate data for the file */
        data = (ramfs_file_data_t*)kmalloc(sizeof(ramfs_file_data_t));
        if (!data) {
            return KERNEL_FAILURE;
        }
        data->content = NULL;
        data->size = 0;
        data->capacity = 0;
        node->data = data;
    }
    /* Ensure we have enough space */
    size_t new_size = offset + count;
    if (new_size > data->capacity) {
        size_t new_capacity = data->capacity;
        if (new_capacity == 0) {
            new_capacity = 256;
        }
        while (new_capacity < new_size) {
            new_capacity *= 2;
        }
        char *new_content = (char*)krealloc(data->content, new_capacity);
        if (!new_content) {
            kfree(data);
            node->data = NULL;
            return KERNEL_FAILURE;
        }
        data->content = new_content;
        data->capacity = new_capacity;
    }
    /* Copy the data */
    memcpy(data->content + offset, buf, count);
    if (new_size > data->size) {
        data->size = new_size;
    }
    return count;
}

static int ramfs_stat(vfs_node_t *node, struct stat *st) {
    if (!st) {
        return KERNEL_FAILURE;
    }
    memset(st, 0, sizeof(struct stat));
    switch (node->type) {
        case VFS_NODE_TYPE_DIRECTORY:
            st->st_mode = S_IFDIR | 0755;
            break;
        case VFS_NODE_TYPE_FILE:
            st->st_mode = S_IFREG | 0644;
            st->st_size = ((ramfs_file_data_t*)node->data)->size;
            st->st_blocks = (st->st_size + 511) / 512;
            break;
        case VFS_NODE_TYPE_DEVICE:
            st->st_mode = S_IFBLK | 0644; /* Example */
            break;
        case VFS_NODE_TYPE_SYMLINK:
            st->st_mode = S_IFLNK | 0755; /* Example */
            break;
        default:
            st->st_mode = 0;
            break;
    }
    return KERNEL_SUCCESS;
}

/* Define the ramfs operations */
static const vfs_ops_t ramfs_ops = {
    .open = ramfs_open,
    .close = ramfs_close,
    .read = ramfs_read,
    .write = ramfs_write,
    .stat = ramfs_stat
};

/* Helper function to normalize a path (remove . and .., and collapse multiple slashes) */
static char* normalize_path(const char *path) {
    /* We'll implement a simple version: just return a copy for now */
    /* In a real implementation, we would handle . and .. and multiple slashes */
    return kstrdup(path);
}

/* Helper function to resolve a path relative to the current working directory */
static vfs_node_t* resolve_path(const char *path) {
    if (!path) {
        return NULL;
    }
    char *norm = normalize_path(path);
    if (!norm) {
        return NULL;
    }

    /* If the path is absolute, start from root; otherwise, start from cwd */
    vfs_node_t *current = NULL;
    if (norm[0] == '/') {
        current = vfs_root;
        norm++; /* Skip the leading slash */
    } else {
        if (!vfs_cwd) {
            kfree(norm);
            return NULL;
        }
        current = vfs_cwd;
    }

    /* Tokenize the path by '/' */
    char *saveptr = NULL;
    char *token = strtok_r(norm, "/", &saveptr);
    while (token) {
        if (strcmp(token, ".") == 0) {
            /* Stay in the same directory */
        } else if (strcmp(token, "..") == 0) {
            if (current->parent) {
                current = current->parent;
            }
            /* If we are at the root, stay at root */
        } else {
            /* Look for a child with this name */
            vfs_node_t *child = vfs_dir_find_child(current, token);
            if (!child) {
                kfree(norm);
                return NULL;
            }
            current = child;
        }
        token = strtok_r(NULL, "/", &saveptr);
    }

    kfree(norm);
    return current;
}

/* VFS initialization */
int vfs_init(void) {
    /* Create the root directory */
    vfs_root = vfs_create_node("/", VFS_NODE_TYPE_DIRECTORY, NULL);
    if (!vfs_root) {
        return KERNEL_FAILURE;
    }
    vfs_root->parent = NULL; /* Root has no parent */
    vfs_cwd = vfs_root; /* Set the current working directory to root */

    /* Initialize the file descriptor table */
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        fd_table[i].node = NULL;
        fd_table[i].flags = 0;
        fd_table[i].offset = 0;
    }
    next_fd = 0; /* We'll start allocating from 0, but we reserve 0,1,2 for stdin,stdout,stderr later */

    return KERNEL_SUCCESS;
}

/* Mount a filesystem (for now, we only support ramfs and we ignore the source and filesystemtype) */
int vfs_mount(const char *source, const char *target, const char *filesystemtype, unsigned long flags, const void *data) {
    /* For simplicity, we only support mounting ramfs at the target */
    /* We'll just ensure the target exists as a directory */
    vfs_node_t *target_node = resolve_path(target);
    if (!target_node) {
        return KERNEL_FAILURE;
    }
    if (target_node->type != VFS_NODE_TYPE_DIRECTORY) {
        return KERNEL_FAILURE;
    }
    /* In a real implementation, we would set the target node's ops to the filesystem's ops */
    /* But for now, we assume that the target node is already using the ramfs ops (which it is by default) */
    return KERNEL_SUCCESS;
}

/* Lookup a node by path */
vfs_node_t* vfs_lookup(const char *path) {
    return resolve_path(path);
}

/* Open a file by path and return a file descriptor */
int vfs_open(const char *path, int flags) {
    vfs_node_t *node = resolve_path(path);
    if (!node) {
        return KERNEL_FAILURE;
    }
    if (node->ops->open(node, flags) != KERNEL_SUCCESS) {
        return KERNEL_FAILURE;
    }
    /* Find a free file descriptor */
    int fd = -1;
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (fd_table[i].node == NULL) {
            fd = i;
            break;
        }
    }
    if (fd == -1) {
        return KERNEL_FAILURE; /* No free file descriptors */
    }
    fd_table[fd].node = node;
    fd_table[fd].flags = flags;
    fd_table[fd].offset = 0;
    return fd;
}

/* Close a file descriptor */
int vfs_close(int fd) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || fd_table[fd].node == NULL) {
        return KERNEL_FAILURE;
    }
    vfs_node_t *node = fd_table[fd].node;
    if (node->ops->close(node) != KERNEL_SUCCESS) {
        return KERNEL_FAILURE;
    }
    fd_table[fd].node = NULL;
    fd_table[fd].flags = 0;
    fd_table[fd].offset = 0;
    return KERNEL_SUCCESS;
}

/* Read from a file descriptor */
ssize_t vfs_read(int fd, void *buf, size_t count) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || fd_table[fd].node == NULL) {
        return KERNEL_FAILURE;
    }
    vfs_node_t *node = fd_table[fd].node;
    off_t offset = fd_table[fd].offset;
    ssize_t bytes_read = node->ops->read(node, buf, count, offset);
    if (bytes_read > 0) {
        fd_table[fd].offset += bytes_read;
    }
    return bytes_read;
}

/* Write to a file descriptor */
ssize_t vfs_write(int fd, const void *buf, size_t count) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || fd_table[fd].node == NULL) {
        return KERNEL_FAILURE;
    }
    vfs_node_t *node = fd_table[fd].node;
    off_t offset = fd_table[fd].offset;
    ssize_t bytes_written = node->ops->write(node, buf, count, offset);
    if (bytes_written > 0) {
        fd_table[fd].offset += bytes_written;
    }
    return bytes_written;
}

/* Stat a file by path */
int vfs_stat(const char *path, struct stat *st) {
    vfs_node_t *node = resolve_path(path);
    if (!node) {
        return KERNEL_FAILURE;
    }
    return node->ops->stat(node, st);
}