#include "monitor.h"

u16int *video_memory = (u16int *)0xB8000;

u16int cursor_x = 0;
u16int cursor_y = 0;
// default color for the VGA
u8int default_color = VGA_COLOR_WHITE | VGA_COLOR_BLACK << 4;

static void move_cursor()
{
    u16int cursorLocation = cursor_y * 80 + cursor_x;
    // select high byte of the cursor location register
    // by writing 14 to I/O port 0x3D4
    outb(0x3D4, 14);
    outb(0x3D5, cursorLocation >> 8);
    // select low byte of cursor location register
    // by writing 15 to I/O Port 0x3D4
    outb(0x3D4, 15);
    // write the low byte of `cursorLocation` to the
    // data register at I/O port 0x3D5
    outb(0x3D5, cursorLocation);
}

static void scroll()
{
    u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    u16int blank = 0x20 /* space */ | (attributeByte << 8);

    if(cursor_y >= 25)
    {
        int i;
        // scroll the screen up
        for (i = 0*80; i < 24*80; i++)
        {
            video_memory[i] = video_memory[i+80];
        }

        // clears the last line on the screen by filling
        // it with the `blank character` from start to end 
        // of the last line
        for (i = 24*80; i < 25*80; i++)
        {
            video_memory[i] = blank;
        }

        // After scrolling, set cursor's y-coordinate to the last
        // line to continue from the start of the newly cleared line
        cursor_y = 24;
    }
}

// function to place character to screen
// with default color (white foreground and 
// black background color)
void monitor_put(char c)
{
    monitor_put_color(c, default_color);
}

// function to place particular character with
// specified color at current cursor position
void monitor_put_color(char c, u8int color)
{
    u16int attribute = color << 8;
    u16int *location;

    // if character is `backspace` and cursor
    // is not at the beginning of the line
    // move cursor one postion to the left
    if (c == 0x08 && cursor_x)
    {
        cursor_x--;
    }
    
    // if the character is `tab` move the cursor 
    // to the next tab stop by aligning the 
    // cursor position to the next multiple of 8
    else if (c == 0x09)
    {
        cursor_x = (cursor_x + 8) & ~(8 - 1);
    }

    // if the character is `carriage return` move the
    // cursor to the start of the current line
    else if (c == '\r')
    {
        cursor_x = 0;
    }

    // if the character is `newline` move the cursor 
    // to the start of the next line 
    else if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
    }

    // if the character is printable character (ASCII)
    // calculate it's position in VGA where it should be 
    // placed and combine the charater with attribute (color)
    // then write to the calculated location
    else if (c >= ' ')
    {
        location = video_memory + (cursor_y * 80 + cursor_x);
        *location = c | attribute;
        cursor_x++;
    }

    // if the cursor reaches the end of line move the cursor
    // to the start of the next line
    if (cursor_x >= 80)
    {
        cursor_x = 0;
        cursor_y++;
    }

    scroll();
    move_cursor();
}

// function that resets the screen to a blank state
void monitor_clear()
{
    u8int attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    u16int blank = 0x20 /* space */ | (attributeByte << 8);

    int i;
    // iterating over the all cells
    // there are 25 rows and 80 columns 
    // in screen text mode, and fill each
    // cell with blank value
    for (i = 0; i < 80*25; i++)
    {
        video_memory[i] = blank;
    }

    // reset the cursor position to the
    // top-left corner with coordinates (0, 0)
    cursor_x = 0;
    cursor_y = 0;
    move_cursor();
}

// function to write null-terminated string
// to the screen
void monitor_write(char *c)
{
    int i = 0;
    while (c[i])
    {
        monitor_put(c[i++]);
    }
}

void monitor_write_hex(u32int n)
{
    s32int tmp;

    monitor_write("0x");

    char noZeroes = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }
    
        if (tmp >= 0xA)
        {
            noZeroes = 0;
            monitor_put(tmp-0xA+'a');
        }
        else
        {
            noZeroes = 0;
            monitor_put(tmp+'0');
        }
    }
  
    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        monitor_put(tmp-0xA+'a');
    }
    else
    {
        monitor_put(tmp+'0');
    }
}

// function converts an unsigned 32-bit integer
// to its decimal string representation and prints 
void monitor_write_dec(u32int n)
{
    if (n == 0)
    {
        monitor_put('0');
        return;
    }

    s32int acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc % 10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    monitor_write(c2);
}

void monitor_write_color(const char *str, u8int fg, u8int bg)
{
    u8int color = fg | (bg << 4);
    int i = 0;
    while (str[i])
    {
        monitor_put_color(str[i++], color);
    }
}

void monitor_write_dec_color(u32int n, u8int fg, u8int bg)
{
    u8int color = fg | (bg << 4);
    if (n == 0)
    {
        monitor_put_color('0', color);
        return;
    }

    s32int acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc % 10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    monitor_write_color(c2, fg, bg);
}

void monitor_set_cursor(u8int x, u8int y)
{
    cursor_x = x;
    cursor_y = y;
    move_cursor();
}

void monitor_set_color(u8int fg, u8int bg)
{
    default_color = fg | (bg << 4);
}
