#include "lib/display.h"
#include "lib/ps2k.h"
#include "lib/hwio.h"

//Entry point of the kernel
void main()
{
	//Variables to 
	unsigned char scanCode = 0;
	unsigned char shift = 0;
	
	
	clearscreen();
	
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
			setcursor_o(getcursor() - 2);
			clearcursor();
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


