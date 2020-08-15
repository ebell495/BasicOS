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

//Loads a function to the specified irq index
//Indices 0 to 47 are taken, so the number has to be between 48 and 255
void interrupt_register_interrupt(unsigned char number, void (*interrupt)(void));

#endif