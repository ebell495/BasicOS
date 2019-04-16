#include "ps2k.h"
#include "hwio.h"
#include "display.h"

const unsigned char key_code_char_ns[256] = 
{
0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,
0,'q','w','e','r','t','y','u','i','o','p','[',']',0,0,
  'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
  'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0
};

const unsigned char key_code_char_shift[256] = 
{
0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,
0,'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,
  'A','S','D','F','G','H','J','K','L',':','\"','~',0,'|',
  'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',0
};

//Gets the character represention of the current key being pressed
//Returns zero if there is no key being pressed
unsigned char getKeyPress()
{
	unsigned char scanCode = getScanCode();
	if(scanCode == 0)
		return 0;
	else if(scanCode > 0x80)
		return 0;
	return getChar(scanCode, 0);
}

//Gets the ps2 code of the key being pressed or released
unsigned char getScanCode()
{
	//Checks if the ps2 buffer has data
	if((pbytein(0x64) & 0x01) == 1)
	{
		//Read the byte in
		unsigned char scanCode = pbytein(0x60);
		return scanCode;
	}
	return 0;
}

//Will wait for a key to be pressed before returning the character representation of the key pressed
unsigned char waitForKeyPress()
{
	unsigned char keyChar = getKeyPress();
	while(keyChar == 0)
	{
		keyChar = getKeyPress();
	}
	return keyChar;
}

//Converts the scanCode to a character for output
//if shift is 0, it will get the lowercase character
unsigned char getChar(unsigned char scanCode, unsigned char shift)
{
	//Note: find a better way to store this information rather than pushing it to the stack
	
	
	if(shift == 0)
		return key_code_char_ns[scanCode];
	else
		return key_code_char_shift[scanCode];
}