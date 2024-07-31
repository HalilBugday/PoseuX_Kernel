#include "scheduler.h"
#include "threads.h"
#include "timer.h"
#include "common.h"
#include "monitor.h"
#include "isr.h"

#define TIME_QUANTUM 100  // 10 ticks

typedef struct priority_queue_node {
    tcb_t *task;
    struct priority_queue_node *next;
} pq_node_t;

static pq_node_t *ready_queue = NULL;
static tcb_t *current_task = NULL;
static u32int quantum_count = 0;

static void enqueue_task(tcb_t *task) {
    pq_node_t *new_node = (pq_node_t *)kmalloc(sizeof(pq_node_t));
    new_node->task = task;
    new_node->next = NULL;

    if (ready_queue == NULL || ready_queue->task->task_priority > task->task_priority) {
        new_node->next = ready_queue;
        ready_queue = new_node;
    } else {
        pq_node_t *current = ready_queue;
        while (current->next != NULL && current->next->task->task_priority <= task->task_priority) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
    monitor_write("Added thread having priority: ");
    monitor_write_dec(task->task_priority);
    monitor_write("\n");
}

static void enqueue_task_end(tcb_t *task) {
    pq_node_t *new_node = (pq_node_t *)kmalloc(sizeof(pq_node_t));
    new_node->task = task;
    new_node->next = NULL;

    if (ready_queue == NULL) {
        ready_queue = new_node;
    } else {
        pq_node_t *current = ready_queue;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    monitor_write("Thread appended back at end having priority: ");
    monitor_write_dec(task->task_priority);
    monitor_write("\n");
}

static tcb_t *dequeue_task() {
    if (ready_queue == NULL) {
        return NULL;
    }
    pq_node_t *node = ready_queue;
    tcb_t *task = node->task;
    ready_queue = ready_queue->next;
    kfree(node);
    monitor_write("Removed thread having priority: ");
    monitor_write_dec(task->task_priority);
    monitor_write("\n");
    return task;
}

void scheduler_add_task(tcb_t *task) {
    enqueue_task(task);
}

void scheduler_yield() {
    if (current_task != NULL) {
        // Update the remaining period of the current task
        current_task->remaining_time -= TIME_QUANTUM;
        monitor_write("Thread having ID: ");
        monitor_write_dec(current_task->task_id);
        monitor_write(", priority: ");
        monitor_write_dec(current_task->task_priority);
        monitor_write(", time left: ");
        monitor_write_dec(current_task->remaining_time);
        monitor_write("\n");

        // If the task has not finished its period, reset its priority to the original
        current_task->task_priority = current_task->original_priority;

        // Re-enqueue the task if its remaining period is not exhausted
        if (current_task->remaining_time > 0) {
            enqueue_task(current_task);
        } else {
            // Reset the task's remaining period to its original period
            current_task->remaining_time = current_task->task_period;
            enqueue_task_end(current_task);
            monitor_write("Thread having ID: ");
            monitor_write_dec(current_task->task_id);
            monitor_write(" has finished executing.\n");
        }
    }

    current_task = dequeue_task();
    quantum_count = 0;  // Reset the quantum count for the new task
    if (current_task != NULL) {
        monitor_write("Thread taking turn having ID: ");
        monitor_write_dec(current_task->task_id);
        monitor_write(", priority: ");
        monitor_write_dec(current_task->task_priority);
        monitor_write("\n");
    }
    // Add delay to make task switching observable
    for (volatile int i = 0; i < 900000000; i++);
}

void scheduler_schedule() {
    quantum_count++;
    if (quantum_count >= TIME_QUANTUM) {
        scheduler_yield();
    }
    if (current_task != NULL) {
        current_task->task_func(current_task->task_arg);
    }
}

void scheduler_init() {
    quantum_count = 0;
    current_task = NULL;
}

static void timer_callback(registers_t regs) {
    scheduler_schedule();
}

void init_scheduler_timer() {
    register_interrupt_handler(IRQ0, &timer_callback);
    init_clock(100);  // Initialize the timer with frequency 100 Hz (10 ms time quantum)
}
