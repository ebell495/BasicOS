#include "lib/klib.h"

//Entry point of the kernel
void main()
{
	//Start functions
	disp_clearscreen();
	idt_init();
	timer_init_timer();
	p_initserial();
	
	p_serial_writestring("TEST STRING");
	
	mem_read_e820();
	ata_initdrive();
	
	//Variables to hold the last scanCode and if the shift key is pressed
	unsigned char scanCode = 0;
	unsigned char shift = 0;
	
	char* test = "Hello, World!";
	
	//Allocate some memory in the heap
	char* r = (char*) kmalloc(20);
	
	disp_printstring(test);
	disp_printc('\n');
	
	//Copy from the string to the pointer set up in the heap
	memcpy(r, test, 20);
	
	disp_printstring(r);
	
	
	disp_printc('\n');
	
	char* alp = kmalloc(28);
	
	//Generate a new string that is the uppercase alphabet
	for(int i = 0; i < 26; i++)
	{
		alp[i] = 'A' + i;
	}
	
	alp[26] = '\n';
	alp[27] = 0;
	
	disp_printstring(alp);
	
	//Read the first sector of the drive
	//The last 2 bytes should be 55AA
	
	unsigned char* read = ata_readsector(0);
	
	//Change the last bytes to FEAB
	//Will break the image so it wouldn't be bootable anymore
	read[510] = 0xFE;
	read[511] = 0xAB;
	
	//Write it back to the 1st sector
	ata_writesector(0, read);
	disp_printc('\n');
	
	//Clear the memory
	for(int j = 0; j < 512; j++)
	{
		read[j] = 0x00;
	}
	
	kfree(read);
	
	//Read back in the sector and check that the last 2 bytes are FEAB
	read = ata_readsector(0);
	disp_phex8(read[510]);
	disp_phex8(read[511]);
	
	//Change them back
	read[510] = 0x55;
	read[511] = 0xAA;
	
	//Write it back to the 1st sector
	ata_writesector(0, read);
	
	int secCount = 0;
	
	while(secCount < 1)
	{
		timer_wait(TICKS_PER_SECOND);
		disp_printstring(" Second: ");
		secCount++;
	}
	
	//Simple write to display loop
	while(1)
	{
		//These are for keys that are released
		while((scanCode =  ps2_getscancode()) == 0 || scanCode > 0x80)
		{
			//These are the shift keys released
			if(scanCode == 0xAA || scanCode == 0xB6)
				shift = 0;
		}
		//Backspace
		if(scanCode == 0x0E)
		{
			//Displays backspace
			disp_backspace();
		}
		//Shift
		else if(scanCode == 0x36 || scanCode == 0x2A)
			shift = 1;
		//Return/Enter key
		else if(scanCode == 0x1C)
		{
			disp_printc('\n');
		}
		//Otherwise its probably a character
		else
		{
			char in = ps2_getchar(scanCode, shift);
			disp_printc(in);
			p_serial_write(in);
		}
	}
}


