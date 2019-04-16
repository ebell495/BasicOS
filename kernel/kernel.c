#include "lib/display.h"
#include "lib/ps2k.h"
#include "lib/hwio.h"
#include "lib/memLib.h"

//Entry point of the kernel
void main()
{
	clearscreen();
	
	//Variables to 
	unsigned char scanCode = 0;
	unsigned char shift = 0;
	
	char* test = "Hello, World!";
	//Make a pointer in some random part of memory
	char* r = (char*) 0x3000;
	
	printString(test);
	printc('\n');
	
	//Copy from the string to the pointer set up at 0x3000
	memcpy(r, test, 20);
	
	printString(r);
	
	//Simple write to display loop
	while(1)
	{
		//These are for keys that are released
		while((scanCode = getScanCode()) == 0 || scanCode > 0x80)
		{
			//These are the shift keys released
			if(scanCode == 0xAA || scanCode == 0xB6)
				shift = 0;
		}
		//Backspace
		if(scanCode == 0x0E)
		{
			//Displays backspace
			backspace();
		}
		//Shift
		else if(scanCode == 0x36 || scanCode == 0x2A)
			shift = 1;
		//Return/Enter key
		else if(scanCode == 0x1C)
		{
			printc('\n');
		}
		//Otherwise its probably a character
		else
			printc(getChar(scanCode, shift));
	}
}


