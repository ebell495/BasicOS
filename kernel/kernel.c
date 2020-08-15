#include "lib/klib.h"
#include "lib/util/bitmap.h"

//Entry point of the kernel

extern void irq80_yield();
extern void irq81_enablePreemtion();
extern void irq82_disablePreemtion();
struct BitmapImage image;

void interrupt_hand1()
{
	unsigned int vgaBuffer = 0;
	while(1)
	{
		//p_serial_printf("P2");
		//disp_printc('2');
		//p_serial_write('2');
		vgaBuffer = vga_getCurrentBuffer();
		vga_draw_bitmap(image.imageBuffer, image.imageWidth, image.imageHeight, 100 + (util_genRand() % 400), 100 + (util_genRand() % 400));

		while(vga_getCurrentBuffer() == vgaBuffer)
		{
			__asm__("int $80");
		}
	}
}

void interrupt_hand2()
{
	while(1)
	{
		//p_serial_printf("P3");
		//disp_printc('3');
		__asm__("int $80");
	}
}

void main()
{
	p_initserial();
	mem_read_e820();
	timer_init_timer();
	idt_init();
	vga_initDisplay();
	__asm__("sti");

	Process* p1 = createProcess(interrupt_hand1, "p1");
	Process* p2 = createProcess(interrupt_hand2, "p2");
	// Process* p3 = createProcess(interrupt_hand2, "p3");
	// Process* p4 = createProcess(interrupt_hand3, "p4");
	
	Process* vgaRefresh = createProcess(vga_sync_thread_func, "vgaRefresh");
	
	// queueProcess(p1);
	// queueProcess(p2);
	// queueProcess(p3);
	// queueProcess(p4);
	// queueProcess(vgaRefresh);

	addProcess(p1);
	addProcess(p2);
	addProcess(vgaRefresh);

	// unsigned int newStackStart = getBitmapStart() - 8;
	// __asm__("movl %0, %%ebp" : : "r"(newStackStart));
	// __asm__("movl %ebp, %esp");

	p_serial_writestring("TEST STRING");
	
	ata_initdrive(); 

	LEAN_readSuperblock();

	openFile("kernel.bin");

	image = loadImage("testImage.bmp");

	interrupt_register_interrupt(80, irq80_yield);
	interrupt_register_interrupt(81, irq81_enablePreemtion);
	interrupt_register_interrupt(82, irq82_disablePreemtion);

	p_serial_printf("Kernel Location: %xi\n", time_getsysticks);
	__asm__("mov $43690, %eax");
	__asm__("mov $48059, %ebx");
	__asm__("mov $52428, %ecx");
	__asm__("mov $56797, %edx");

	enableScheduling();

	while(1)
	{
		//timer_wait(100);
		// p_serial_printf("Kernel Location: %xi\n", main);
	 // 	__asm__("mov $43690, %eax");
		//  __asm__("mov $48059, %ebx");
		//  __asm__("mov $52428, %ecx");
		//  __asm__("mov $56797, %edx");

		// //unsigned int startTick = time_getsysticks();

		// // vga_draw_bitmap(image.imageBuffer, image.imageWidth, image.imageHeight, 100, 100);
		// // vga_swap_buffers_noclr();

		// // unsigned int endTick = time_getsysticks();
	 // // 	p_serial_printf("Frame Time %i\n", endTick - startTick);
	 // // 	timer_wait(500);

		// vga_setpixel_rgb(util_genRand() % 1024, util_genRand() % 768, util_genRand() % 256, util_genRand() % 256, util_genRand() % 256);

		// if(count == 100)
		// {
		// 	vga_swap_buffers_noclr();
		// 	count = 0;
		// }

		// count++;

		// if(util_genRand() == 853654723)
		// 	isRunning = 0;
	}
}



