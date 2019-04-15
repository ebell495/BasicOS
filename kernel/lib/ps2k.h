#ifndef _ps2kh
#define _ps2kh
unsigned char getKeyPress();
unsigned char waitForKeyPress();
unsigned char getChar(unsigned char scanCode, unsigned char shift);
unsigned char getScanCode();
#endif