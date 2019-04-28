#include "lib/display.h"
#include "lib/ps2k.h"
#include "lib/hwio.h"
#include "lib/memlib.h"

//Entry point of the kernel
void main()
{
	//Start functions
	disp_clearscreen();
	mem_read_e820();
	
	//Variables to 
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
	
	p_initserial();
	
	disp_printc('\n');
	
	char* alp = kmalloc(28);
	
	for(int i = 0; i < 26; i++)
	{
		alp[i] = 'A' + i;
	}
	
	alp[26] = '\n';
	alp[27] = 0;
	
	disp_printstring(alp);
	
	kfree(alp);

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


