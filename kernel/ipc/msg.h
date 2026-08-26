#ifndef _KERNEL_IPC_MSG_H
#define _KERNEL_IPC_MSG_H

#include <stdint.h>
#include <stddef.h>
#include "../syscall.h"

// Maximum number of message queues in the system
#define MSGQUEUE_MAX 16

// Maximum size of a message (in bytes)
#define MSG_SIZE_MAX 4096

// Message structure
struct msg {
    long mtype;           // Message type (must be > 0)
    char mtext[MSG_SIZE_MAX]; // Message text
};

// Message queue structure
struct msg_queue {
    key_t key;                    // Key for lookup
    int id;                       // Queue ID
    struct msg messages[MSGQUEUE_MAX]; // Fixed-size array of messages
    int head;                     // Index of the first message
    int tail;                     // Index of the next free slot
    int count;                    // Number of messages in the queue
    uint32_t pid_list[16];        // Simple list of PIDs that have queued or dequeued (for notifications)
    int pid_count;
    // We'll add locking later; for now, we assume single-threaded or disable interrupts
};

// Function prototypes
// Function prototypes
int sys_msgget(key_t key, int msgflg);
int sys_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
int sys_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
int sys_msgctl(int msqid, int cmd, struct msqid_ds *buf);

// Initialize the IPC subsystem
void msg_init(void);

#endif /* _KERNEL_IPC_MSG_H */