#include "threads.h"
#include "timer.h"
#include "scheduler.h"
#include "common.h"

static u32int task_id_counter = 1;

void create_task(tcb_t *task, void (*task_func)(void *), void *task_arg) {
    task->task_func = task_func;
    task->task_arg = task_arg;
    task->task_id = task_id_counter++;
    task->stack = (u32int *)kmalloc(task->stack_sz); // Allocate stack memory
    task->remaining_time = task->task_period; // Initialize remaining time to the task's period
    task->original_priority = task->task_priority; // Initialize original priority
    // Initialize stack and registers for the task (not shown here)
    scheduler_add_task(task);
}
