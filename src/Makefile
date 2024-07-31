SOURCES=boot.o main.o monitor.o common.o descriptor_tables.o isr.o interrupt.o gdt.o timer.o cli.o kb.o scheduler.o threads.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32 -O0 
LDFLAGS=-melf_i386 -Tlink.ld 
ASFLAGS=-felf32

all: $(SOURCES) link

clean:
	-rm *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
