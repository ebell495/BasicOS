#ifndef _interruptsh
#define _interruptsh

struct IDT_entry
{
	unsigned short offset_low;
	unsigned short selector;
	unsigned char zero;
	unsigned char type_attrib;
	unsigned short offset_high;
};

void idt_init();

void registerISV(unsigned char irqNum, int (*isvFunc)(void));

#endif