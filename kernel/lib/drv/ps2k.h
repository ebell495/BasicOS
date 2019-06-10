#ifndef _ps2kh
#define _ps2kh

//ps2_getkeypress
//Gets the ASCII representation of the key being pressed
//Does not wait if there are no keys being pressed
//Returns: The ASCII character of the key being pressed or 0 if no key is pressed
unsigned char ps2_getkeypress();

//ps2_waitforkeypress
//Gets the ASCII representation of the key being pressed
//Will wait if there are no keys being pressed until a key is pressed
//Returns: The ASCII character of the key being pressed or 0 scancode is not a pressed scancode
unsigned char ps2_waitforkeypress();

//ps2_getchar
//Param: Scancode-The ps2 scancode, shift-boolean to get either get the lowercase(0) or uppercase(1) character
//Converts the ps2 scancode into the ASCII character representaion
//Returns: The ASCII character of the scancode or 0 if the code is not a character
unsigned char ps2_getchar(unsigned char scanCode, unsigned char shift);

//ps2_getscancode
//Reads the scancode from the ps2 buffer
//Returns: The scancode from the ps2 keyboard or 0 if there was no code available
unsigned char ps2_getscancode();
#endif