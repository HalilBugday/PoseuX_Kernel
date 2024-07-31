#include "common.h"
#include "monitor.h"
#include "timer.h"
#include "scheduler.h"

extern u16int cursor_x;
extern u16int cursor_y;
extern void run_progs();

void shell()
{
    char command[128];
    monitor_write_color("OSx383> ", VGA_COLOR_CYAN, VGA_COLOR_BLACK);

    while (1)
    {
        monitor_set_cursor(9, cursor_y);
        gets(command);
        monitor_put_color('\n', VGA_COLOR_WHITE | (VGA_COLOR_BLACK << 4));
        if (strcmp(command, "help") == 0)
        {
            monitor_write_color("Available commands: 'help', 'settime', 'clear', 'run threads', 'reboot'\n\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
        else if (strcmp(command, "clear") == 0)
        {
            monitor_clear();
        }
        else if (strncmp(command, "settime", 7) == 0)
        {
            u8int hour = 0, minute = 0, second = 0;
            int i = 7, result = 0;
            int values[3] = {0, 0, 0};
            int valueIndex = 0;
            int currentValue = 0;

            while (command[i] == ' ')
            {
                i++;
            }
            while (command[i] != '\0')
            {
                if (command[i] >= '0' && command[i] <= '9')
                {
                    currentValue = currentValue * 10 + (command[i] - '0');
                }
                else if (command[i] == ' ' || command[i + 1] == '\0')
                {
                    if (command[i + 1] == '\0' && command[i] >= '0' && command[i] <= '9')
                    {
                        currentValue = currentValue * 10 + (command[i] - '0');
                    }
                    if (valueIndex < 3)
                    {
                        values[valueIndex++] = currentValue;
                        currentValue = 0;
                    }
                    if (command[i] == ' ')
                    {
                        while (command[i] == ' ')
                        {
                            i++;
                        }
                        i--;
                    }
                }
                else
                {
                    result = -1;
                    break;
                }
                if (command[i + 1] == '\0' && valueIndex < 3)
                {
                    values[valueIndex++] = currentValue;
                    currentValue = 0;
                    break;
                }
                i++;
            }
            if (valueIndex == 3 && result == 0)
            {
                hour = values[0];
                minute = values[1];
                second = values[2];

                if (hour > 23 || minute > 59 || second > 59)
                {
                    monitor_write_color("Given time range invalid!\n\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                }
                else
                {
                    clock_time_t new_time;
                    new_time.hrs = hour;
                    new_time.mins = minute;
                    new_time.secs = second;
                    write_clock(&new_time);

                    monitor_write_color("Time is set to: ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    if (hour < 10)
                        monitor_write_color("0", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    monitor_write_dec_color(hour, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    monitor_write_color(":", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    if (minute < 10)
                        monitor_write_color("0", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    monitor_write_dec_color(minute, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    monitor_write_color(":", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    if (second < 10)
                        monitor_write_color("0", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    monitor_write_dec_color(second, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                    monitor_write_color("\n\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
                }
            }
            else
            {
                monitor_write_color("Usage: settime hh mm ss\n\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
        }
        else if (strcmp(command, "reboot") == 0)
        {
            monitor_write_color("\nRebooting...\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            for (int j = 2; j > 0; j--)
            {
                for (int k = 0; k < 999999999; k++);
            }
            asm volatile("cli");
            asm volatile("outb %%al, $0x64" : : "a"(0xFE));
            while (1)
            {
                asm volatile("hlt");
            }
        }
        else if (strcmp(command, "run threads") == 0)
        {
            run_progs();
        }
        else
        {
            monitor_write_color("Invalid command. Type 'help' for a list of available commands\n\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
        monitor_write_color("OSx383> ", VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    }
}
