#include "timer.h"
#include "monitor.h"
#include "scheduler.h"

u32int timer_ticks = 0;

extern u16int cursor_x;
extern u16int cursor_y;

u8int read_cmos_reg(u8int reg)
{
    outb(CMOS_ADDR, reg); 
    return inb(CMOS_DATA);  
}

u8int get_update_flag()
{
    outb(CMOS_ADDR, 0x0A);
    return inb(CMOS_DATA) & 0x80;
}

u8int convert_from_bcd(u8int value)
{
    return (value & 0x0F) + ((value / 16) * 10);
}

u8int convert_to_bcd(u8int value)
{
    return ((value / 10) << 4) | (value % 10);
}

void read_clock(clock_time_t *time)
{
    while (get_update_flag())
    {}

    u8int last_secs, last_mins, last_hrs;
    u8int secs, mins, hrs;
    do {
        last_secs = secs;
        last_mins = mins;
        last_hrs = hrs;

        while (get_update_flag()) {}

        secs = read_cmos_reg(0x00);
        mins = read_cmos_reg(0x02);
        hrs = read_cmos_reg(0x04);
    } while (last_secs != secs || last_mins != mins || last_hrs != hrs);

    time->secs = secs;
    time->mins = mins;
    time->hrs = hrs;
    u8int regB = read_cmos_reg(0x0B);
    if (!(regB & 0x04))
    {
        time->secs = convert_from_bcd(time->secs);
        time->mins = convert_from_bcd(time->mins);
        time->hrs = convert_from_bcd(time->hrs);
        if (!(regB & 0x02) && (time->hrs & 0x80))
        {
            time->hrs = ((time->hrs & 0x7F) + 12) % 24;
        }
    }
}

void write_clock(const clock_time_t *time)
{
    while (get_update_flag())
    {}

    u8int regB = read_cmos_reg(0x0B);

    u8int secs = time->secs;
    u8int mins = time->mins;
    u8int hrs = time->hrs;

    if (!(regB & 0x04))
    {
        secs = convert_to_bcd(secs);
        mins = convert_to_bcd(mins);
        hrs = convert_to_bcd(hrs);
        if (!(regB & 0x02) && (hrs > 12))
        {
            hrs = (hrs % 12) | 0x80;
        }
    }

    outb(CMOS_ADDR, 0x00);
    outb(CMOS_DATA, secs);
    outb(CMOS_ADDR, 0x02);
    outb(CMOS_DATA, mins);
    outb(CMOS_ADDR, 0x04);
    outb(CMOS_DATA, hrs);
}

static void clock_interrupt_handler(registers_t regs)
{
    timer_ticks++;
    if (timer_ticks % 18 == 0)
    {
        clock_time_t current_time;
        read_clock(&current_time);
        if (current_time.hrs > 23 || current_time.mins > 59 || current_time.secs > 59)
        {
            monitor_write_color("Given time range is invalid.\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
        else
        {
            current_time.secs++;
            if (current_time.secs > 59)
            {
                current_time.secs = 0;
                current_time.mins++;
            }
            if (current_time.mins > 59)
            {
                current_time.mins = 0;
                current_time.hrs++;
            }
            if (current_time.hrs > 23)
            {
                current_time.hrs = 0;
            }
        }

        u8int saved_cursor_x = cursor_x;
        u8int saved_cursor_y = cursor_y;
        u8int cursor_start_x = 66;
        u8int cursor_start_y = 0;

        monitor_set_cursor(cursor_start_x, cursor_start_y);
        for (int i = 0; i < 12; i++)
        {
            monitor_put_color(' ', VGA_COLOR_YELLOW | (VGA_COLOR_LIGHT_GREY << 4));
        }
        monitor_set_cursor(cursor_start_x, cursor_start_y);

        if (current_time.hrs < 10)
            monitor_put_color('0', VGA_COLOR_YELLOW | (VGA_COLOR_LIGHT_GREY << 4));
        monitor_write_dec_color(current_time.hrs, VGA_COLOR_YELLOW, VGA_COLOR_LIGHT_GREY);
        monitor_put_color(':', VGA_COLOR_YELLOW | (VGA_COLOR_LIGHT_GREY << 4));
        if (current_time.mins < 10)
            monitor_put_color('0', VGA_COLOR_YELLOW | (VGA_COLOR_LIGHT_GREY << 4));
        monitor_write_dec_color(current_time.mins, VGA_COLOR_YELLOW, VGA_COLOR_LIGHT_GREY);
        monitor_put_color(':', VGA_COLOR_YELLOW | (VGA_COLOR_LIGHT_GREY << 4));
        if (current_time.secs < 10)
            monitor_put_color('0', VGA_COLOR_YELLOW | (VGA_COLOR_LIGHT_GREY << 4));
        monitor_write_dec_color(current_time.secs, VGA_COLOR_YELLOW, VGA_COLOR_LIGHT_GREY);

        monitor_set_cursor(saved_cursor_x, saved_cursor_y);
    }
}

void init_clock(u32int freq)
{
    register_interrupt_handler(IRQ0, &clock_interrupt_handler);

    u32int divisor = 1193180 / freq;
    outb(0x43, 0x36);
    u8int l = (u8int)(divisor & 0xFF);
    u8int h = (u8int)((divisor >> 8) & 0xFF);
    outb(0x40, l);
    outb(0x40, h);
}

void set_clock_period(unsigned int period) {
    u32int frequency = 1000000 / period;
    init_clock(frequency);
}

void wait_clock_event() {
    asm volatile ("hlt");
    scheduler_yield();
}
