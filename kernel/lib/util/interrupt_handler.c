#include "../drv/hwio.h"
#include "../drv/display.h"
#include "../drv/ata.h"
#include "timer.h"
#include "../drv/ps2k.h"
#include "../thread/process.h"
#include "memlib.h"
#include "../thread/sched.h"

//Taken from https://wiki.osdev.org/Interrupts_tutorial

#define IRQ0_COUNT_FIRE 1

struct Process proc;

//System Timer
void irq0_handler() 
{
	int scanCode = 0xDE;
	doSwapInterrupt();
	timer_timeinterrupt();
	//p_serial_printf("IRQ0 ");
	pbyteout(0x20, 0x20); //EOI
}

int shift = 0;

//Keyboard
void irq1_handler() 
{
	//disp_printstring("IRQ1 ");

	int scanCode = ps2_getscancode();
	// scanCode = 0xDEAD;

	// for(int i = 0; i < 64; i++)
	// {
	// 	p_serial_printf("%xi: %xi\n", (unsigned int*)((&scanCode + i)), *(unsigned int*)((&scanCode + i)));
	// }

	// p_serial_printf("irq0_handler Location: %xi\n", irq0_handler);

	// memcpy(&(proc.savedState.registers), (unsigned int*)((&scanCode + 7)), 11*4);
	// proc.savedState.registers.esp += 12;

	// p_serial_printf("%xi, %xi, %xi", proc.savedState.registers.eax, proc.savedState.registers.eip, proc.savedState.registers.edx);

	// if(proc.savedState.savedStack == 0)
	// 	proc.savedState.savedStack = kmalloc(1024);

	// memcpy(proc.savedState.savedStack, (unsigned char*) proc.savedState.registers.esp, proc.savedState.registers.ebp - proc.savedState.registers.esp);
	// dumpMemLoc(proc.savedState.savedStack, 64);

	//doSwapInterrupt();

	//Backspace
	if(scanCode == 0x0E)
	{
		//Displays backspace
		disp_backspace();
	}
	//Shift
	else if(scanCode == 0x36 || scanCode == 0x2A)
	{
		shift = 1;
	}
	//Return/Enter key
	else if(scanCode == 0x1C)
	{
		//disp_printc('\n');
	}
	else if(scanCode == 0x48)
	{
		//disp_printc('a');
		//disp_movecursor(0, 1);
	}
	else if(scanCode == 0x4D)
	{
		//disp_printc('b');
	}
	else if(scanCode == 0x50)
	{
		//disp_printc('c');
		//disp_movecursor(0, -1);
	}
	else if(scanCode == 0x4B)
	{	
		//disp_printc('d');
	}
	//Otherwise its probably a character
	else
	{
		char in = ps2_getchar(scanCode, shift);
		//disp_phex32(scanCode);
		disp_printc(in);
		//p_serial_write(in);
		//p_serial_writenum(time_getsysticks() & 0xFFFFFFFF);
		p_serial_write(in);
	}

	pbyteout(0x20, 0x20); //EOI
}

//
void irq2_handler() 
{
	disp_printstring("IRQ2 ");
	pbyteout(0x20, 0x20); //EOI
}

//COM 2
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

//LPT 2&3 OR Sound Card
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

//ACPI Devices
void irq9_handler() 
{
	disp_printstring("IRQ9 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}

//Open Interrupt
void irq10_handler() 
{
	disp_printstring("IRQ10 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}

//Open Interrupt
void irq11_handler() 
{
	disp_printstring("IRQ11 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}

//Mouse on PS2 connector
void irq12_handler() 
{
	disp_printstring("IRQ12 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}

//FPU, CPU Co-processor, Inter-process interrupt
void irq13_handler() 
{
	disp_printstring("IRQ13 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}

//ATA Primary Drive
void irq14_handler() 
{
	//disp_printstring("IRQ14 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}

//ATA Secondary Drive
void irq15_handler() 
{
	disp_printstring("IRQ15 ");
	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}

void irq_yield_preempt()
{
	doSwapInterrupt();

	pbyteout(0xA0, 0x20);
	pbyteout(0x20, 0x20); //EOI
}