#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"

// defining colors to be used 
// for text output on shell
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_YELLOW 14
#define VGA_COLOR_WHITE 15

// functions for manipulating monitor
void monitor_put(char);
void monitor_clear();
void monitor_write(char *);
void monitor_put_color(char, u8int);
void monitor_write_color(const char *, u8int, u8int);
void monitor_write_hex(u32int);
void monitor_write_dec(u32int);
void monitor_write_dec_color(u32int, u8int, u8int);
void monitor_set_cursor(u8int, u8int);
void monitor_set_color(u8int, u8int);

#endif
