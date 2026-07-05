#ifndef KERNEL_PROCESS_SCHEDULER_H
#define KERNEL_PROCESS_SCHEDULER_H

#include <stdint.h>
#include "../include/kernel.h"

/* Priority levels */
typedef enum {
    PRIORITY_REALTIME = 0,
    PRIORITY_HIGH = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_LOW = 3,
    PRIORITY_COUNT
} process_priority_t;

/* Process state */
typedef enum {
    PROCESS_STATE_RUNNING = 0,
    PROCESS_STATE_READY = 1,
    PROCESS_STATE_SLEEPING = 2,
    PROCESS_STATE_BLOCKED = 3,
    PROCESS_STATE_ZOMBIE = 4,
    PROCESS_STATE_TERMINATED = 5
} process_state_t;

/* Process Control Block (PCB) */
typedef struct process {
    uint32_t pid;                     /* Process ID */
    char name[32];                    /* Process name */
    process_state_t state;            /* Current state */
    process_priority_t priority;      /* Scheduling priority */
    uint32_t cpu_time_ms;             /* Accumulated CPU time */
    uint32_t memory_pages;            /* Memory usage in pages */
    struct process *next;             /* Link to next process in list */
    struct process *prev;             /* Link to previous process (for doubly linked list) */
    void *stack_ptr;                  /* Stack pointer for context switching */
    void *entry_point;                /* Entry point of the process */
    void *arg;                        /* Argument passed to entry point */
    /* IPC and other fields can be added later */
} process_t;

/* Scheduler functions */
int scheduler_init(void);
process_t *scheduler_create_process(const char *name, void (*entry)(void *), void *arg, process_priority_t priority);
void scheduler_destroy_process(process_t *proc);
void scheduler_yield(void);
void scheduler_tick(uint32_t delta_ms); /* Called by timer interrupt */
process_t *scheduler_get_current_process(void);
void scheduler_set_current_process(process_t *proc);
void scheduler_dump_state(void);

#endif /* KERNEL_PROCESS_SCHEDULER_H */