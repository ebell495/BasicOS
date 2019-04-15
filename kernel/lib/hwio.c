#include "hwio.h"

//Outputs a byte to the port
void pbyteout(unsigned short port, unsigned char data)
{
	__asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}

//Reads in a byte from the port given
unsigned char pbytein(unsigned short port)
{
	unsigned char result;
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

