#ifndef COMMON_H
#define COMMON_H

#include "stdtypes.h"

#define NULL ((void*)0)

typedef unsigned int u32int;
typedef int s32int;
typedef unsigned short u16int;
typedef short s16int;
typedef unsigned char u8int;
typedef char s8int;

void outb(u16int, u8int);
u8int inb(u16int );
u16int inw(u16int );

void memcpy(u8int *,const u8int *,u32int);
void memset(u8int *, u8int, u32int);
char *strcpy(char *,const char *);
char *strcat(char *,const char *);
char *gets(char *); 
int strcmp(char *, char *);
int strncmp( const char *,const char *, size_t);
size_t strlen(const char *);
void itoa(int ,char *,int );
char *gets(char *);
void monitor_write_position(const char*, u8int, u8int, u8int, u8int);

void *kmalloc(unsigned int size); 
void kfree(void *ptr);

#endif
