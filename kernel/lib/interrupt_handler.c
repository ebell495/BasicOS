#include "hwio.h"
#include "display.h"

//Taken from https://wiki.osdev.org/Interrupts_tutorial

//System Timer
void irq0_handler() 
{
	//disp_printstring("IRQ0 ");
	pbyteout(0x20, 0x20); //EOI
}

//Keyboard
void irq1_handler() 
{
	//disp_printstring("IRQ1 ");
	pbyteout(0x20, 0x20); //EOI
}


void irq2_handler() 
{
	disp_printstring("IRQ2 ");
	pbyteout(0x20, 0x20); //EOI
}
 
void irq3_handler() 
{
	disp_printstring("IRQ3 ");
	pbyteout(0x20, 0x20); //EOI
}

//COM1
void irq4_handler() 
{
	disp_printstring("IRQ4 ");
	pbyteout(0x20, 0x20); //EOI
}
 
void irq5_handler() 
{
	disp_printstring("IRQ5 ");
	pbyteout(0x20, 0x20); //EOI
}

//Floppy
void irq6_handler() 
{
	disp_printstring("IRQ6 ");
	pbyteout(0x20, 0x20); //EOI
}

//LPT1
void irq7_handler() 
{
	disp_printstring("IRQ7 ");
	pbyteout(0x20, 0x20); //EOI
}

//CMOS Real Time Clock
void irq8_handler() 
{
	disp_printstring("IRQ8 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI          
}
 
void irq9_handler() 
{
	disp_printstring("IRQ9 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}
 
void irq10_handler() 
{
	disp_printstring("IRQ10 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}
 
void irq11_handler() 
{
	disp_printstring("IRQ11 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}
 
void irq12_handler() 
{
	disp_printstring("IRQ12 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}
 
void irq13_handler() 
{
	disp_printstring("IRQ13 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}

//ATA Hard drive
void irq14_handler() 
{
	//disp_printstring("IRQ14 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}
 
void irq15_handler() 
{
	disp_printstring("IRQ15 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}