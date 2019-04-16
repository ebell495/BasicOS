#include "display.h"
#include "hwio.h"

#define VMEM_LOCATION 0xB8000

#define REG_SCREEN_CTR 0x3D4
#define REG_SCREEN_DATA 0x3D5

//Calulates the memory offset from the row and column information
int vOffset(int row, int col)
{
	return ((80*row) + col)*2;
}

//Prints a character to the screen at the cursor location
//Will automatically move the cursor to the next position
void printc(unsigned char c)
{
	if(c == 0)
		return;
	
	int offset = getcursor();
	
	if(c=='\n')
	{
		int rows = offset / (2*80);
		offset = vOffset(rows+1, 0);
		setcursor_o(offset);
		return;
	}
	
	if((offset % 80) == 79)
	{
		int rows = offset / (2*80);
		offset = vOffset(rows+1, 0);
	}
	
	unsigned char* vMemLoc = (unsigned char*)(VMEM_LOCATION);
	vMemLoc[offset] = c;
	vMemLoc[offset+1] = 0x0F;

	setcursor_o(offset + 2);
}

//Sets the cursor to the row and column given
void setcursor_rc(int row, int col)
{
	int offset = (row*80)+col;
	
	pbyteout(REG_SCREEN_CTR, 0x0E);
	pbyteout(REG_SCREEN_DATA, (unsigned char) ((offset >> 8) & 0xFF));
	
	pbyteout(REG_SCREEN_CTR, 0x0F);
	pbyteout(REG_SCREEN_DATA, (unsigned char) (offset & 0xFF));
}

//Sets the cursor to the memory offset given
//Note: this is the total memory offset meaning it is both the character and color information
void setcursor_o(int offset)
{
	offset /= 2;
	
	pbyteout(REG_SCREEN_CTR, 0x0E);
	pbyteout(REG_SCREEN_DATA, (unsigned char) ((offset >> 8) & 0xFF));
	
	pbyteout(REG_SCREEN_CTR, 0x0F);
	pbyteout(REG_SCREEN_DATA, (unsigned char) (offset & 0xFF));
}

//Gets the memory location of the cursor position
unsigned int getcursor()
{
	unsigned int offset = 0;
	
	pbyteout(REG_SCREEN_CTR, 0x0E);
	
	offset = pbytein(REG_SCREEN_DATA) << 8;
	
	pbyteout(REG_SCREEN_CTR, 0x0F);
	
	offset += pbytein(REG_SCREEN_DATA);
	offset *= 2;
	return offset;
}

//Clears the screen buffer
void clearscreen()
{
	unsigned char* vMem = (unsigned char*)0xB8000;
	for(int i = 0; i < 25; i++)
	{
		for(int j = 0; j < 80; j++)
		{
			*vMem = 0x00;
			vMem++;
			*vMem = 0x0F;
			vMem++;
		}
	}
	setcursor_o(0);
}

//Prints the hex representation of the short passed
void pHex16(unsigned short x)
{
	for(int i = 12; i >= 0; i-=4)
	{
		unsigned char d = (x >> i) & 0x0F;
		d += '0';
		if(d > '9')
			d += 7;
		printc(d);
	}
}

//Prints the hex representation of the integer passed
void pHex32(unsigned int x)
{
	for(int i = 28; i >= 0; i-=4)
	{
		unsigned char d = (x >> i) & 0x0F;
		d += '0';
		if(d > '9')
			d += 7;
		printc(d);
	}
}

//Prints the hex representation of the byte passed
void pHex8(unsigned char x)
{
	for(int i = 4; i >= 0; i-=4)
	{
		unsigned char d = (x >> i) & 0x0F;
		d += '0';
		if(d > '9')
			d += 7;
		printc(d);
	}
}

//Clears the location under the cursor
void clearcursor()
{
	int offset = getcursor();
	unsigned char* vMemLoc = (unsigned char*)(VMEM_LOCATION);
	vMemLoc[offset] = 0;
	vMemLoc[offset+1] = 0;
}