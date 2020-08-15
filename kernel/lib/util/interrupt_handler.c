#include "../drv/hwio.h"
#include "../drv/display.h"
#include "../drv/ata.h"
#include "timer.h"
#include "../drv/ps2k.h"
#include "../thread/process.h"
#include "memlib.h"
#include "../thread/sched.h"

//Taken from https://wiki.osdev.org/Interrupts_tutorial

unsigned long long system_tick_counter = 0;
unsigned int system_sched_tick_counter = SCHED_TICK;

void exception_handler(unsigned int errorCode, unsigned int exceptionNum)
{
	p_serial_printf("EXCEPTION: %i : ERROR CODE: %xi : EIP: %xi\n", exceptionNum, errorCode, *((unsigned int*)(&exceptionNum + 9)));

	*((unsigned int*)(&exceptionNum + 9)) += 2;
}

//System Timer
void irq0_handler(unsigned int* contextStart)
{
	system_sched_tick_counter++;

	if(system_sched_tick_counter > SCHED_TICK && getSchedulingStatus())
	{
		system_sched_tick_counter = 0;
		//p_serial_printf("SWAP\n");
		doScheduleEvent(contextStart);
	}

	timer_timeinterrupt();
}

int shift = 0;

//Keyboard
void irq1_handler() 
{
	int scanCode = ps2_getscancode();

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

}

//
void irq2_handler() 
{
	disp_printstring("IRQ2 ");
}

//COM 2
void irq3_handler() 
{
	disp_printstring("IRQ3 ");
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
}

//Floppy
void irq6_handler() 
{
	disp_printstring("IRQ6 ");
}

//LPT1
void irq7_handler() 
{
	disp_printstring("IRQ7 ");
}

//CMOS Real Time Clock
void irq8_handler() 
{
	disp_printstring("IRQ8 ");      
}

//ACPI Devices
void irq9_handler() 
{
	disp_printstring("IRQ9 ");
}

//Open Interrupt
void irq10_handler() 
{
	disp_printstring("IRQ10 ");
}

//Open Interrupt
void irq11_handler() 
{
	disp_printstring("IRQ11 ");
}

//Mouse on PS2 connector
void irq12_handler() 
{
	disp_printstring("IRQ12 ");
}

//FPU, CPU Co-processor, Inter-process interrupt
void irq13_handler() 
{
	disp_printstring("IRQ13 ");
}

//ATA Primary Drive
void irq14_handler() 
{
	//disp_printstring("IRQ14 ");
}

//ATA Secondary Drive
void irq15_handler() 
{
	disp_printstring("IRQ15 ");
}

//INT 80, forces the scheduler to reschedule before the given quantum is up
void irq_yield_preempt_handler(unsigned int* contextStart)
{
	//doSwapInterrupt(1);
	system_sched_tick_counter = 0;
	doScheduleEvent(contextStart);
}

void irq_enablePreemtion_handler()
{
	enableScheduling();
}

void irq_disablePreemtion_handler()
{
	disableScheduling();
}