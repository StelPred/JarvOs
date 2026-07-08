#include "scheduler.h"
#include "../include/kernel.h"
#include <string.h>
#include "../memory/vmm.h"
#include "../memory/memory.h"
#include "../memory/pmm.h"

#define PROCESS_STACK_SIZE 4096

extern void context_switch(uint32_t **old_esp_ptr, uint32_t *new_esp);
extern uint64_t tick_count; // ticks since boot, updated every timer interrupt

static void process_entry_trampoline(void) {
    process_t *proc = scheduler_get_current_process();
    void (*entry)(void*) = proc->entry_point;
    void *arg = proc->arg;
    entry(arg);
    proc->state = PROCESS_STATE_TERMINATED;
    for (;;) { __asm__ __volatile__("hlt"); }
}

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
static process_t *sleep_list = NULL; // tasks waiting for their wake-up time

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

static void enqueue_sleep(process_t *proc) {
    proc->next = sleep_list;
    sleep_list = proc;
}

static void check_sleepers(void) {
    process_t *prev = NULL;
    process_t *curr = sleep_list;
    while (curr != NULL) {
        process_t *next = curr->next;
        if (tick_count >= curr->wake_tick) {
            if (prev == NULL) sleep_list = next;
            else prev->next = next;
            curr->state = PROCESS_STATE_READY;
            enqueue_process(curr);
        } else {
            prev = curr;
        }
        curr = next;
    }
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

process_t *scheduler_create_process(const char *name, void (*entry)(void *), void *arg, process_priority_t priority) {
    process_t *proc = allocate_process();
    if (proc == NULL) {
        return NULL;
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
    extern uint32_t* vmm_create_address_space(void);
    proc->page_directory = vmm_create_address_space();
    proc->entry_point = entry;
    proc->arg = arg;
   uint32_t stack_phys = pmm_alloc_page();
    if (stack_phys == 0) {
        release_process(proc);
        return NULL;
    }
    uint8_t *stack = (uint8_t*)stack_phys;
    proc->stack_phys = stack_phys;
    uint32_t *saved_dir = vmm_get_current_directory();
    if (proc->page_directory != NULL) {
        vmm_switch_directory(proc->page_directory);
    }
    vmm_map_page(stack, stack_phys, PAGE_WRITABLE);
    vmm_switch_directory(saved_dir);
    
    uint32_t *sp = (uint32_t*)(stack + PROCESS_STACK_SIZE);
    *(--sp) = (uint32_t)process_entry_trampoline;
    *(--sp) = 0; // ebp
    *(--sp) = 0; // ebx
    *(--sp) = 0; // esi
    *(--sp) = 0; // edi
    proc->stack_ptr = sp;
    proc->next = NULL;
    proc->prev = NULL;

    // Add the process to the appropriate priority queue
    enqueue_process(proc);
    
    return proc;
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

void scheduler_sleep_current(uint32_t ms) {
    if (current_process == NULL || current_process == &idle_process) {
        return;
    }
    uint32_t ticks = ms / 10; // timer fires every 10ms
    if (ticks == 0) ticks = 1;
    current_process->wake_tick = tick_count + ticks;
    current_process->state = PROCESS_STATE_SLEEPING;

    while (current_process->state == PROCESS_STATE_SLEEPING) {
        __asm__ __volatile__("hlt");
    }
}

void scheduler_tick(uint32_t delta_ms) {
    process_t *prev = current_process;

   check_sleepers();

    if (current_process != NULL && current_process->state == PROCESS_STATE_RUNNING) {
        current_process->cpu_time_ms += delta_ms;
        enqueue_process(current_process);
        current_process = NULL;
    } else if (current_process != NULL && current_process->state == PROCESS_STATE_SLEEPING) {
        current_process->cpu_time_ms += delta_ms;
        enqueue_sleep(current_process);
        current_process = NULL;
    }
    process_t *next = pick_next_process();
    if (next != NULL) {
        dequeue_process(next->priority);
        current_process = next;
    } else {
        current_process = &idle_process;
    }

    if (prev != current_process) {
        extern void vmm_switch_directory(uint32_t* page_dir);
        if (current_process->page_directory != NULL) {
            vmm_switch_directory(current_process->page_directory);
        }
        context_switch((uint32_t**)&prev->stack_ptr, (uint32_t*)current_process->stack_ptr);
    }
}

process_t *scheduler_fork_process(process_t *parent) {
    return scheduler_create_process(parent->name, parent->entry_point, parent->arg, parent->priority);
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