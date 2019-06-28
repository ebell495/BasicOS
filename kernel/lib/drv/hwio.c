#include "hwio.h"

#define PORT 0x3f8 //COM1

//Outputs a byte to the port
void pbyteout(unsigned short port, unsigned char data)
{
	__asm__("outb %%al, %%dx" : :"a" (data), "d" (port));
}

//Reads in a byte from the port given
unsigned char pbytein(unsigned short port)
{
	unsigned char result;
	__asm__("inb %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

//Outputs a word(16 bits) to the port
void pwordout(unsigned short port, unsigned short data)
{
	__asm__("outw %%ax, %%dx" : :"a" (data), "d" (port));
}

//Reads in a word(16 bits) from the port given
unsigned short pwordin(unsigned short port)
{
	unsigned short result;
	__asm__("inw %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

void p_initserial()
{
	pbyteout(PORT + 1, 0x00);    // Disable all interrupts
	pbyteout(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	pbyteout(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	pbyteout(PORT + 1, 0x00);    //                  (hi byte)
	pbyteout(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
	pbyteout(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	pbyteout(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int p_serial_received()
{
	return pbytein(PORT + 5) & 1;
}

char p_serial_read()
{
	//Wait for data
	if(p_serial_received() == 0)
	{
		return 0;
	}
	
	return pbytein(PORT);
}

int p_transmit_empty()
{
	return pbytein(PORT + 5) & 0x20;
}

void p_serial_write(char byte)
{
	while (p_transmit_empty() == 0);
 
	pbyteout(PORT,byte);
}

void p_serial_writestring(char* string)
{
	while(*string != '\0')
	{
		p_serial_write(*string);
		string++;
	}
}

void p_serial_writebytes(char* bytes, unsigned int size)
{
	for(int i = 0; i < size; i++)
	{
		p_serial_write(bytes[i]);
	}
}

void p_serial_writenum(long num)
{
	if(num < 0)
	{
		p_serial_write('-');
		num *= -1;
	}
	
	if(num > 9)
		p_serial_writenum(num / 10);
	
	p_serial_write('0' + (num % 10));
}


