#include "interrupts.h"
#include "hwio.h"
#include "display.h"

struct IDT_entry IDT[256];


//Taken from https://wiki.osdev.org/Interrupts_tutorial
void idt_init()
{
	//disp_phex32(sizeof (struct IDT_entry));
	extern int load_idt();
	extern int irq0();
	extern int irq1();
	extern int irq2();
	extern int irq3();
	extern int irq4();
	extern int irq5();
	extern int irq6();
	extern int irq7();
	extern int irq8();
	extern int irq9();
	extern int irq10();
	extern int irq11();
	extern int irq12();
	extern int irq13();
	extern int irq14();
	extern int irq15();
 
	unsigned long irq0_address;
	unsigned long irq1_address;
	unsigned long irq2_address;
	unsigned long irq3_address;          
	unsigned long irq4_address; 
	unsigned long irq5_address;
	unsigned long irq6_address;
	unsigned long irq7_address;
	unsigned long irq8_address;
	unsigned long irq9_address;          
	unsigned long irq10_address;
	unsigned long irq11_address;
	unsigned long irq12_address;
	unsigned long irq13_address;
	unsigned long irq14_address;          
	unsigned long irq15_address;         
	unsigned long idt_address;
	unsigned long idt_ptr[2];
	
	//Prepare the PIC to be remapped
	pbyteout(0x20, 0x11);
	pbyteout(0xA0, 0x11);
	pbyteout(0x21, 0x20);
	pbyteout(0xA1, 0x28);
	pbyteout(0x21, 0x04);
	pbyteout(0xA1, 0x02);
	pbyteout(0x21, 0x01);
	pbyteout(0xA1, 0x01);
	pbyteout(0x21, 0x0);
	pbyteout(0xA1, 0x0);
	
	irq0_address = (unsigned long)irq0; 
	IDT[32].offset_low = irq0_address & 0xffff;
	IDT[32].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[32].zero = 0;
	IDT[32].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[32].offset_high = (irq0_address & 0xffff0000) >> 16;
 
	irq1_address = (unsigned long)irq1; 
	IDT[33].offset_low = irq1_address & 0xffff;
	IDT[33].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[33].zero = 0;
	IDT[33].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[33].offset_high = (irq1_address & 0xffff0000) >> 16;
 
	irq2_address = (unsigned long)irq2; 
	IDT[34].offset_low = irq2_address & 0xffff;
	IDT[34].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[34].zero = 0;
	IDT[34].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[34].offset_high = (irq2_address & 0xffff0000) >> 16;
 
	irq3_address = (unsigned long)irq3; 
	IDT[35].offset_low = irq3_address & 0xffff;
	IDT[35].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[35].zero = 0;
	IDT[35].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[35].offset_high = (irq3_address & 0xffff0000) >> 16;
 
	irq4_address = (unsigned long)irq4; 
	IDT[36].offset_low = irq4_address & 0xffff;
	IDT[36].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[36].zero = 0;
	IDT[36].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[36].offset_high = (irq4_address & 0xffff0000) >> 16;
 
	irq5_address = (unsigned long)irq5; 
	IDT[37].offset_low = irq5_address & 0xffff;
	IDT[37].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[37].zero = 0;
	IDT[37].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[37].offset_high = (irq5_address & 0xffff0000) >> 16;
 
	irq6_address = (unsigned long)irq6; 
	IDT[38].offset_low = irq6_address & 0xffff;
	IDT[38].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[38].zero = 0;
	IDT[38].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[38].offset_high = (irq6_address & 0xffff0000) >> 16;
 
	irq7_address = (unsigned long)irq7; 
	IDT[39].offset_low = irq7_address & 0xffff;
	IDT[39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[39].zero = 0;
	IDT[39].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[39].offset_high = (irq7_address & 0xffff0000) >> 16;
 
	irq8_address = (unsigned long)irq8; 
	IDT[40].offset_low = irq8_address & 0xffff;
	IDT[40].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[40].zero = 0;
	IDT[40].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[40].offset_high = (irq8_address & 0xffff0000) >> 16;
 
	irq9_address = (unsigned long)irq9; 
	IDT[41].offset_low = irq9_address & 0xffff;
	IDT[41].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[41].zero = 0;
	IDT[41].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[41].offset_high = (irq9_address & 0xffff0000) >> 16;
 
	irq10_address = (unsigned long)irq10; 
	IDT[42].offset_low = irq10_address & 0xffff;
	IDT[42].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[42].zero = 0;
	IDT[42].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[42].offset_high = (irq10_address & 0xffff0000) >> 16;
 
	irq11_address = (unsigned long)irq11; 
	IDT[43].offset_low = irq11_address & 0xffff;
	IDT[43].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[43].zero = 0;
	IDT[43].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[43].offset_high = (irq11_address & 0xffff0000) >> 16;
 
	irq12_address = (unsigned long)irq12; 
	IDT[44].offset_low = irq12_address & 0xffff;
	IDT[44].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[44].zero = 0;
	IDT[44].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[44].offset_high = (irq12_address & 0xffff0000) >> 16;
 
	irq13_address = (unsigned long)irq13; 
	IDT[45].offset_low = irq13_address & 0xffff;
	IDT[45].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[45].zero = 0;
	IDT[45].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[45].offset_high = (irq13_address & 0xffff0000) >> 16;
 
	irq14_address = (unsigned long)irq14; 
	IDT[46].offset_low = irq14_address & 0xffff;
	IDT[46].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[46].zero = 0;
	IDT[46].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[46].offset_high = (irq14_address & 0xffff0000) >> 16;
 
    irq15_address = (unsigned long)irq15; 
	IDT[47].offset_low = irq15_address & 0xffff;
	IDT[47].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[47].zero = 0;
	IDT[47].type_attrib = 0x8e; /* INTERRUPT_GATE */
	IDT[47].offset_high = (irq15_address & 0xffff0000) >> 16;
 
	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;
 
 

	load_idt(idt_ptr);
 
}