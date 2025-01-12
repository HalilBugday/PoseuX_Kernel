#include "common.h"
#include "isr.h"
#include "monitor.h"

// array to hold interrupt handlers
isr_t interrupt_handlers[256];

// function to register interrupt handler
// with given interrupt number
void register_interrupt_handler(u8int n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

// function to execute specific interrupt 
void isr_handler(registers_t regs)
{
    monitor_write("recieved interrupt: ");
    monitor_write_dec(regs.int_no);
    monitor_put('\n');

    if (interrupt_handlers[regs.int_no] != 0)
    {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
}

// function for handling IRQs by sending EOI 
// (End of Interrupt) signals to PICs (Programmable Interrupt Controllers)
// and calling appropriate ISR if one exists
void irq_handler(registers_t regs)
{
    if (regs.int_no >= 40)
    {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);

    if (interrupt_handlers[regs.int_no] != 0)
    {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }

}
