#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include "threads.h"

// Function prototypes for the scheduler
void scheduler_add_task(tcb_t *task);
void scheduler_yield(void);
void scheduler_schedule(void);
void scheduler_init(void);
void init_scheduler_timer(void);

#endif
