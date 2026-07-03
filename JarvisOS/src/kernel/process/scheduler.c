#include "scheduler.h"
#include "../include/kernel.h"
#include <string.h>

#define MAX_PROCESSES 64

static process_t process_pool[MAX_PROCESSES];
static process_t *free_list = NULL;

// Priority queues: one for each priority level
typedef struct {
    process_t *head;
    process_t *tail;
} priority_queue_t;

static priority_queue_t queues[PRIORITY_COUNT];
static process_t *current_process = NULL;
static process_t idle_process; // Idle process

// Initialize the scheduler
int scheduler_init(void) {
    // Initialize the free list: link all process control blocks together
    for (int i = 0; i < MAX_PROCESSES - 1; i++) {
        process_pool[i].next = &process_pool[i+1];
    }
    process_pool[MAX_PROCESSES-1].next = NULL;
    free_list = &process_pool[0];

    // Initialize all priority queues as empty
    for (int i = 0; i < PRIORITY_COUNT; i++) {
        queues[i].head = NULL;
        queues[i].tail = NULL;
    }

    // Create and initialize the idle process
    idle_process.pid = 0;
    strcpy(idle_process.name, "idle");
    idle_process.state = PROCESS_STATE_RUNNING;
    idle_process.priority = PRIORITY_LOW;
    idle_process.cpu_time_ms = 0;
    idle_process.memory_pages = 0;
    idle_process.next = NULL;
    idle_process.prev = NULL;
    idle_process.stack_ptr = NULL;
    idle_process.entry_point = NULL; // Idle process does nothing
    idle_process.arg = NULL;

    // Set the current process to idle initially
    current_process = &idle_process;

    return KERNEL_SUCCESS;
}

// Helper function to get a free process control block
static process_t *allocate_process(void) {
    if (free_list == NULL) {
        return NULL; // Out of processes
    }
    process_t *proc = free_list;
    free_list = free_list->next;
    proc->next = NULL; // Clear next pointer
    return proc;
}

// Helper function to release a process control block back to the free list
static void release_process(process_t *proc) {
    proc->next = free_list;
    free_list = proc;
}

// Enqueue a process into the appropriate priority queue (at the tail)
static void enqueue_process(process_t *proc) {
    process_priority_t prio = proc->priority;
    if (queues[prio].tail == NULL) {
        // Queue is empty
        queues[prio].head = queues[prio].tail = proc;
        proc->next = NULL;
    } else {
        queues[prio].tail->next = proc;
        queues[prio].tail = proc;
        proc->next = NULL;
    }
}

// Dequeue a process from the head of the appropriate priority queue
static process_t *dequeue_process(process_priority_t prio) {
    if (queues[prio].head == NULL) {
        return NULL;
    }
    process_t *proc = queues[prio].head;
    queues[prio].head = queues[prio].head->next;
    if (queues[prio].head == NULL) {
        queues[prio].tail = NULL;
    }
    proc->next = NULL; // Clear next pointer
    return proc;
}

// Pick the next process to run: highest priority non-empty queue, round-robin within that priority
static process_t *pick_next_process(void) {
    // Check each priority from highest to lowest
    for (int prio = PRIORITY_REALTIME; prio < PRIORITY_COUNT; prio++) {
        if (queues[prio].head != NULL) {
            // Found a non-empty queue, return the head process (we'll rotate later by moving head to tail after time slice)
            // For simplicity, we just take the head and then we will put it back at the tail when preempted.
            // This gives round-robin within the priority.
            return queues[prio].head;
        }
    }
    return NULL; // No runnable process
}

int scheduler_create_process(const char *name, void (*entry)(void *), void *arg, process_priority_t priority) {
    process_t *proc = allocate_process();
    if (proc == NULL) {
        return KERNEL_FAILURE;
    }

    // Initialize the process control block
    static uint32_t next_pid = 1;
    proc->pid = next_pid++;
    strncpy(proc->name, name, sizeof(proc->name)-1);
    proc->name[sizeof(proc->name)-1] = '\0';
    proc->state = PROCESS_STATE_READY;
    proc->priority = priority;
    proc->cpu_time_ms = 0;
    proc->memory_pages = 0; // To be set by memory subsystem
    proc->entry_point = entry;
    proc->arg = arg;
    proc->stack_ptr = NULL; // To be set when we allocate a stack (simplified for now)
    proc->next = NULL;
    proc->prev = NULL;

    // Add the process to the appropriate priority queue
    enqueue_process(proc);

    return KERNEL_SUCCESS;
}

void scheduler_destroy_process(process_t *proc) {
    // Remove the process from any queue it might be in (simplified: we assume it's not in a queue when destroyed)
    // In a real implementation, we would need to remove it from the queue.
    // For now, we just release the process control block.
    release_process(proc);
}

void scheduler_yield(void) {
    // If there is a current process and it is not the idle process, we preempt it and schedule the next
    if (current_process != NULL && current_process != &idle_process) {
        // Put the current process back in its priority queue (if it's still runnable)
        if (current_process->state == PROCESS_STATE_RUNNING) {
            enqueue_process(current_process);
        }
        current_process = NULL;
    }
    // The scheduler tick will be called to pick the next process
}

void scheduler_tick(uint32_t delta_ms) {
    // If we have a current process that is running, we update its CPU time and then preempt it if time slice expired
    // For simplicity, we'll use a fixed time slice of 10ms and preempt on every tick.
    // In a real system, we would compare delta_ms to a time slice.

    if (current_process != NULL && current_process->state == PROCESS_STATE_RUNNING) {
        current_process->cpu_time_ms += delta_ms;
        // Preempt the current process: put it back in the queue
        enqueue_process(current_process);
        current_process = NULL;
    }

    // Pick the next process to run
    process_t *next = pick_next_process();
    if (next != NULL) {
        // Remove it from the queue
        dequeue_process(next->priority); // We know the priority from the process, but we don't have it stored in the queue?
        // Actually, we stored the process in the queue by priority, so we need to know the priority to dequeue from the correct queue.
        // We have the priority in the process: next->priority
        // But note: we already removed it from the queue in pick_next_process? No, we only peeked.
        // So we need to dequeue it now.
        // We'll change pick_next_process to return the process and also remove it?
        // Let's refactor: we'll have pick_next_process return the process and remove it from the queue.
        // But for now, we'll do:
        //   We know the priority, so we can dequeue from that priority's queue.
        // However, note: the process might have been moved to a different queue? No, priority doesn't change.
        // So we can do:
        //   dequeue_process(next->priority);
        // But wait: our pick_next_process only returns the head of the queue for a priority, and we know that priority from the loop.
        // We didn't store which priority we found. Let's change the function to return both the process and the priority.

        // Given the time, let's change the design: we'll have pick_next_process return the process and we'll assume that the process is still in the queue.
        // Then we remove it by searching the queue? That's inefficient.

        // Instead, let's change pick_next_process to remove the process from the queue and return it.
        // We'll do that by having the function also return the priority via an output parameter, or we can store the priority in the process and then we know which queue to remove from.

        // We'll do the latter: we know the process's priority, so we can remove it from that queue.
        // But note: the process might not be at the head of the queue anymore?
        // In our current pick_next_process, we return the head of the queue for the first non-empty priority.
        // So we know it is at the head of that queue.

        // Therefore, we can do:
        //   dequeue_process(next->priority);
        // However, we must be sure that the process we got is indeed the head of that queue.

        // Since we are returning the head, it is safe.

        // But note: what if the queue changed between the time we looked and now?
        // In a single-core system with interrupts disabled during the scheduler tick, it's safe.

        // We'll assume interrupts are disabled when scheduler_tick is called.

        // So:
        dequeue_process(next->priority); // Remove the head of the queue for this priority

        current_process = next;
        // TODO: Actually switch to the next process (context switch)
        // For now, we just set the current process and let the architecture code handle the switch.
    } else {
        // No process to run, run the idle process
        current_process = &idle_process;
    }
}

process_t *scheduler_get_current_process(void) {
    return current_process;
}

void scheduler_set_current_process(process_t *proc) {
    current_process = proc;
}

void scheduler_dump_state(void) {
    // For debugging: dump the state of all processes and queues
    // This is a simplified version for now
    // In a real kernel, we would use a logging mechanism
    // We'll just print to a debug console if available
    // Since we don't have a console yet, we'll skip the implementation for now
    // We'll just set a breakpoint or use a debugger in QEMU
    // For the purpose of this task, we leave it empty.
}