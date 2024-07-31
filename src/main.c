#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "cli.h"
#include "kb.h"
#include "isr.h"
#include "threads.h"
#include "scheduler.h"
#include "common.h"

void delay() {
    for (volatile int i = 0; i < 1000000; i++);
}
void Task(void *arg) {
    tcb_t *task = (tcb_t *)arg;
    while (1) {
        delay();
        scheduler_yield();
    }
}

void run_progs() {
    scheduler_init();

    tcb_t t3;
    t3.stack_sz = 256;
    t3.task_period = 600;
    t3.task_priority = 3;
    monitor_write("Thread3 Ready! -> ");
    create_task(&t3, Task, &t3);

    tcb_t t1;
    t1.stack_sz = 256;
    t1.task_period = 300;
    t1.task_priority = 1;
    monitor_write("Thread1 Ready! -> ");
    create_task(&t1, Task, &t1);

    tcb_t t4;
    t4.stack_sz = 256;
    t4.task_period = 700;
    t4.task_priority = 4;
    monitor_write("Thread4 Ready! -> ");
    create_task(&t4, Task, &t4);

    tcb_t t2;
    t2.stack_sz = 256;
    t2.task_period = 400;
    t2.task_priority = 2;
    monitor_write("Thread2 Ready! -> ");
    create_task(&t2, Task, &t2);


    monitor_write("\n");
    monitor_write("*************Queue is ready!***************\n");
    monitor_write("**********Threads are ready for scheduling!**********\n");
    monitor_write("\n");
    while (1) {
        scheduler_schedule();
    }
}

int main(struct multiboot *mboot_ptr) {

    init_descriptor_tables();
    monitor_clear();
    keyboard_install();
    monitor_write_color("Welcome to OSX383 Kernel!\n\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    init_clock(100);
    asm volatile("sti");
    shell();
    return 0;
}
