#ifndef TIMER_H
#define TIMER_H

#include "common.h"
#include "isr.h"

// CMOS port addresses
#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

// Struct to represent RTC time
typedef struct {
    u8int secs;
    u8int mins;
    u8int hrs;
} clock_time_t;

// Function prototypes for RTC
void read_clock(clock_time_t *time);
void write_clock(const clock_time_t *time);
u8int get_update_flag();
u8int convert_from_bcd(u8int value);
u8int convert_to_bcd(u8int value);

// Function prototypes for Timer
void init_clock(u32int freq);
void set_clock_period(unsigned int period);
void wait_clock_event();

#endif
