#include "lib/klib.h"
#include "lib/util/bitmap.h"

//Entry point of the kernel

extern int testInt();
struct BitmapImage image;

void interrupt_hand()
{
	unsigned long long lastTick = 0;
	while(1)
	{
		if(lastTick + 100 < time_getsysticks())
		{
			lastTick = time_getsysticks();
			//p_serial_printf("P1 %i\n", lastTick);
			vga_setpixel_rgb(util_genRand() % 1024, util_genRand() % 768, util_genRand() % 256, util_genRand() % 256, util_genRand() % 256);
		}
		else
		{
			//Forces the kernel to schedule tasks before the given timeslice is finished
			
		}
		__asm__("int $80");
	}
}

void interrupt_hand2()
{
	unsigned long long lastTick = 0;
	while(1)
	{
		if(lastTick + 3000 < time_getsysticks())
		{
			lastTick = time_getsysticks();
			p_serial_printf("P3 %i\n", lastTick);
		}
		else
		{
			
		}
		__asm__("int $80");
	}
}

void interrupt_hand3()
{
	unsigned long long lastTick = 0;
	//struct BitmapImage image = loadImage("testImage.bmp");
	while(1)
	{
		if(lastTick + 100 < time_getsysticks())
		{
			lastTick = time_getsysticks();
			//p_serial_printf("P4 %i\n", lastTick);
			vga_draw_bitmap(image.imageBuffer, image.imageWidth, image.imageHeight, 100, 100);
		}
		else
		{
			
		}
		__asm__("int $80");
	}
}

void interrupt_hand1()
{
	unsigned long long lastTick = 0;
	while(1)
	{
		if(lastTick + 1000 < time_getsysticks())
		{
			lastTick = time_getsysticks();
			p_serial_printf("P2 %i\n", lastTick);
			//vga_swap_buffers_noclr();
		}
		else
		{
			
		}
		__asm__("int $80");
	}
}

void main()
{
	p_initserial();
	mem_read_e820();
	init_sched();
	Process* p1 = createProcess(interrupt_hand, "p1");
	Process* p2 = createProcess(interrupt_hand1, "p2");
	Process* p3 = createProcess(interrupt_hand2, "p3");
	Process* p4 = createProcess(interrupt_hand3, "p4");
	
	Process* vgaRefresh = createProcess(vga_sync_thread_func, "vgaRefresh");
	
	queueProcess(p1);
	queueProcess(p2);
	queueProcess(p3);
	queueProcess(p4);
	queueProcess(vgaRefresh);

	// unsigned int newStackStart = getBitmapStart() - 8;
	// __asm__("movl %0, %%ebp" : : "r"(newStackStart));
	// __asm__("movl %ebp, %esp");

	vga_initDisplay();
	idt_init();
	timer_init_timer();

	p_serial_writestring("TEST STRING");
	
	ata_initdrive(); 

	LEAN_readSuperblock();

	openFile("kernel.bin");

	image = loadImage("testImage.bmp");

	registerISV(80, testInt);

	char isRunning = 1;
	unsigned int count = 0;

	p_serial_printf("Kernel Location: %xi\n", main);
	__asm__("mov $43690, %eax");
	__asm__("mov $48059, %ebx");
	__asm__("mov $52428, %ecx");
	__asm__("mov $56797, %edx");
	enablePreempt();

	while(1)
	{
		//timer_wait(100);
		p_serial_printf("Kernel Location: %xi\n", main);
	 	__asm__("mov $43690, %eax");
		 __asm__("mov $48059, %ebx");
		 __asm__("mov $52428, %ecx");
		 __asm__("mov $56797, %edx");

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



