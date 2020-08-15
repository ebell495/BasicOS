#include "interrupts.h"
#include "../drv/hwio.h"
#include "../drv/display.h"

struct IDT_entry IDT[256];

//Taken from https://wiki.osdev.org/Interrupts_tutorial
void idt_init()
{
	extern void irq0();
	extern void irq1();
	extern void irq2();
	extern void irq3();
	extern void irq4();
	extern void irq5();
	extern void irq6();
	extern void irq7();
	extern void irq8();
	extern void irq9();
	extern void irq10();
	extern void irq11();
	extern void irq12();
	extern void irq13();
	extern void irq14();
	extern void irq15();

	extern void exp0();
	extern void exp1();
	extern void exp2();
	extern void exp3();
	extern void exp4();
	extern void exp5();
	extern void exp6();
	extern void exp7();
	extern void exp8();
	extern void exp9();
	extern void exp10();
	extern void exp11();
	extern void exp12();
	extern void exp13();
	extern void exp14();
	extern void exp16();
	extern void exp17();
	extern void exp18();
	extern void exp19();
	extern void exp20();
	extern void exp30();
	
	//Prepare the PIC to be remapped
	//Init command to both PICs
	pbyteout(0x20, 0x11);
	pbyteout(0xA0, 0x11);

	//PIC1 gets offset 0x20 (32)
	pbyteout(0x21, 0x20);
	//PIC2 gets offset 0x28 (40)
	pbyteout(0xA1, 0x28);
	//Tell PIC1 that there is another PIC at IRQ2 (0x04 = 0000 0100)
	pbyteout(0x21, 0x04);
	pbyteout(0xA1, 0x02);
	pbyteout(0x21, 0x01);
	pbyteout(0xA1, 0x01);
	pbyteout(0x21, 0x0);
	pbyteout(0xA1, 0x0);

	interrupt_register_interrupt(0, exp0);
	interrupt_register_interrupt(1, exp1);
	interrupt_register_interrupt(2, exp2);
	interrupt_register_interrupt(3, exp3);
	interrupt_register_interrupt(4, exp4);
	interrupt_register_interrupt(5, exp5);
	interrupt_register_interrupt(6, exp6);
	interrupt_register_interrupt(7, exp7);
	interrupt_register_interrupt(8, exp8);
	interrupt_register_interrupt(9, exp9);
	interrupt_register_interrupt(10, exp10);
	interrupt_register_interrupt(11, exp11);
	interrupt_register_interrupt(12, exp12);
	interrupt_register_interrupt(13, exp13);
	interrupt_register_interrupt(14, exp14);
	interrupt_register_interrupt(16, exp16);
	interrupt_register_interrupt(17, exp17);
	interrupt_register_interrupt(18, exp18);
	interrupt_register_interrupt(19, exp19);
	interrupt_register_interrupt(20, exp20);
	interrupt_register_interrupt(30, exp30);

	interrupt_register_interrupt(32, irq0);
	interrupt_register_interrupt(33, irq1);
	interrupt_register_interrupt(34, irq2);
	interrupt_register_interrupt(35, irq3);
	interrupt_register_interrupt(36, irq4);
	interrupt_register_interrupt(37, irq5);
	interrupt_register_interrupt(38, irq6);
	interrupt_register_interrupt(39, irq7);
	interrupt_register_interrupt(40, irq8);
	interrupt_register_interrupt(41, irq9);
	interrupt_register_interrupt(42, irq10);
	interrupt_register_interrupt(43, irq11);
	interrupt_register_interrupt(44, irq12);
	interrupt_register_interrupt(45, irq13);
	interrupt_register_interrupt(46, irq14);
	interrupt_register_interrupt(47, irq15);
 
}

void interrupt_register_interrupt(unsigned char number, void (*interrupt)(void))
{
	extern void load_idt();
	unsigned long idt_address;
	unsigned long idt_ptr[2];
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	unsigned long isvAddress = (unsigned long)interrupt; 
	IDT[number].offset_low = isvAddress & 0xffff;
	IDT[number].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[number].zero = 0;
	IDT[number].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[number].offset_high = (isvAddress & 0xffff0000) >> 16;

	load_idt(idt_ptr);
}