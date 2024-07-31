#ifndef TASKS_HANDLER_H
#define TASKS_HANDLER_H

#include "common.h"

typedef struct task_control_block {
    void (*task_func)(void *);
    void *task_arg;
    u32int stack_sz;
    u32int *stack;
    u32int task_period;
    u32int remaining_time;  // Paraphrased field name for remaining period
    u32int task_priority;
    u32int original_priority;  // Paraphrased field name for base priority
    int task_id; 
    struct task_control_block *next_task;
} tcb_t;

void create_task(tcb_t *task, void (*task_func)(void *), void *task_arg);

#endif
