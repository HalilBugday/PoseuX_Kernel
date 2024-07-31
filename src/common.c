#include "common.h"
#include "monitor.h"
#include "kb.h"

extern u16int cursor_x;
extern u16int cursor_y;

#define HEAP_SIZE 1024 * 1024 //heap 1 mb

static unsigned char heap[HEAP_SIZE]; //array
static unsigned int heap_index = 0; //initial 

typedef struct free_block {  //for memory alloc
    unsigned int size;
    struct free_block *next;
} free_block_t;

static free_block_t *free_list = NULL; //pointer 

void outb(u16int port, u8int value) //write data
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

u8int inb(u16int port) // I/O port read data
{
    u8int ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

u16int inw(u16int port)// I/O port read data
{
    u16int ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void memcpy(u8int *dest, const u8int *src, u32int len) //copy block of the memory
{
    const u8int *sp = (const u8int *)src;
    u8int *dp = (u8int *)dest;
    for(; len != 0; len--) *dp++ = *sp++;
}

void memset(u8int *dest, u8int val, u32int len) //set memory
{
    u8int *temp = (u8int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

int strcmp(char *str1, char *str2) //compare 2 strings
{
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

size_t strlen(const char *str) //string length
{
    size_t len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}

char *strcpy(char *dest, const char *src) //string copy
{
    char *temp = dest;
    while ((*dest++ = *src++) != '\0');
    return temp;
}

char *strcat(char *dest, const char *src) //concatenates 2 strings
{
    char *temp = dest;
    while (*dest) {
        dest++;
    }
    while ((*dest++ = *src++) != '\0');
    return temp;
}

char *gets(char *buffer) 
{
    char *buf_ptr = buffer;
    char c;
    while ((c = getchar()) != '\n')
    {
        if (c == '\b' && buf_ptr > buffer)
        {
            monitor_put(c);
            buf_ptr--;
        }
        else
        {
            monitor_put(c);
            *buf_ptr++ = c;
        }
    }
    *buf_ptr = '\0';
    return buffer;
}

int strncmp(const char *str1, const char *str2, size_t n) //compare 2 strings (in range n char)
{
    for (size_t i = 0; i < n; i++)
    {
        if (str1[i] != str2[i] || str1[i] == '\0')
        {
            return str1[i] - str2[i];
        }
    }
    return 0;
}

void itoa(int value, char *str, int base) //integer to string
{
    char *rc = str;
    char *ptr;
    char *low;
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return;
    }

    int is_negative = 0;
    if (value < 0 && base == 10)
    {
        is_negative = 1;
        value = -value;
    }

    ptr = str;

    do
    {
        int digit = value % base;
        *ptr++ = (digit > 9) ? (digit - 10) + 'a' : digit + '0';
    } while (value /= base);

    if (is_negative)
    {
        *ptr++ = '-';
    }

    *ptr-- = '\0';

    while (rc < ptr)
    {
        char tmp = *rc;
        *rc++ = *ptr;
        *ptr-- = tmp;
    }
}

void monitor_write_position(const char* str, u8int fore_color, u8int back_color, u8int x, u8int y) {
    u16int saved_cursor_x = cursor_x;
    u16int saved_cursor_y = cursor_y;
    monitor_set_cursor(x, y);

    while (*str) {
        monitor_put_color(*str++, fore_color | (back_color << 4));
    }
    monitor_set_cursor(saved_cursor_x, saved_cursor_y);
}

void *kmalloc(unsigned int size) {
    size = (size + 7) & ~7;
    free_block_t **current = &free_list;
    while ((*current)) {
        if ((*current)->size >= size) {
            void *ptr = *current;
            *current = (*current)->next;
            return ptr;
        }
        current = &(*current)->next;
    }

    if (heap_index + size > HEAP_SIZE) {
        return NULL;
    }

    void *ptr = &heap[heap_index];
    heap_index += size;
    return ptr;
}

void kfree(void *ptr) {
    free_block_t *block = (free_block_t *)ptr;
    block->size = 0; 
    block->next = free_list;
    free_list = block;
}