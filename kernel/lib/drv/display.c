#include "display.h"
#include "hwio.h"
#include "../util/memlib.h"

#define VMEM_LOCATION 0xB8000

#define REG_SCREEN_CTR 0x3D4
#define REG_SCREEN_DATA 0x3D5

char lineEnding[25];

void disp_scroll()
{
	for(int line = 1; line < 25; line++)
	{
		memcpy((char*)VMEM_LOCATION + ((line-1)*160), (char*)VMEM_LOCATION + ((line)*160), 160);
		lineEnding[line-1] = lineEnding[line];
	}
	
	unsigned char* vMemLoc = (unsigned char*)(VMEM_LOCATION) + (24 * 160);
	
	for(int endRow = 0; endRow < 80; endRow++)
	{
		vMemLoc[endRow*2] = 0x0;
		vMemLoc[endRow*2 + 1] = 0x0;
	}
	
	lineEnding[24] = 0;
	disp_setcursor_rc(24, 0);
}

//Calulates the memory offset from the row and column information
int disp_vOffset(int row, int col)
{
	return ((80*row) + col)*2;
}

//Prints a character to the screen at the cursor location
//Will automatically move the cursor to the next position
void disp_printc(unsigned char c)
{
	if(c == 0)
		return;
	
	int offset = disp_getcursor();
	int rows = offset / (2*80);
	if(rows > 24)
	{
		disp_scroll();
		offset = disp_getcursor();
		rows = offset / (2*80);
	}
	
	if(c=='\n')
	{
		offset = disp_vOffset(rows+1, 0);
		disp_setcursor_o(offset);
		lineEnding[rows]++;
		if(rows+1 > 24)
		{
			disp_scroll();
			offset = disp_getcursor();
			rows = offset / (2*80);
		}
		return;
	}
	
	if((offset % 80) == 79)
	{
		offset = disp_vOffset(rows+1, 0);
	}
	
	unsigned char* vMemLoc = (unsigned char*)(VMEM_LOCATION);
	vMemLoc[offset] = c;
	vMemLoc[offset+1] = 0x0F;

	disp_setcursor_o(offset + 2);
	lineEnding[rows]++;
}

//Prints a null-terminated string
void disp_printstring(char* string)
{
	while(*string != '\0')
	{
		disp_printc(*string);
		string++;
	}
}

//Handles the backspace
//Move the cursor back and clears it
//If it was at the end of the line, then it moves the cursor to the end of the last line and clears it
void disp_backspace()
{
	int offset = disp_getcursor();
	int row = offset / (2*80);
	int col = (offset/2)%80;
	
	if(offset == 0)
		return;
	
	if(lineEnding[row] == 0)
	{
		row--;
		col = lineEnding[row];
		
	}
	
	if(col == 0)
	{
		disp_setcursor_rc(row, col);
		disp_clearcursor();
		return;
	}
	
	disp_setcursor_rc(row, col-1);
	disp_clearcursor();
	lineEnding[row]--;
}

//Sets the cursor to the row and column given
void disp_setcursor_rc(int row, int col)
{
	int offset = (row*80)+col;
	
	pbyteout(REG_SCREEN_CTR, 0x0E);
	pbyteout(REG_SCREEN_DATA, (unsigned char) ((offset >> 8) & 0xFF));
	
	pbyteout(REG_SCREEN_CTR, 0x0F);
	pbyteout(REG_SCREEN_DATA, (unsigned char) (offset & 0xFF));
}

//Sets the cursor to the memory offset given
//Note: this is the total memory offset meaning it is both the character and color information
void disp_setcursor_o(int offset)
{
	offset /= 2;
	
	pbyteout(REG_SCREEN_CTR, 0x0E);
	pbyteout(REG_SCREEN_DATA, (unsigned char) ((offset >> 8) & 0xFF));
	
	pbyteout(REG_SCREEN_CTR, 0x0F);
	pbyteout(REG_SCREEN_DATA, (unsigned char) (offset & 0xFF));
}

//Gets the memory location of the cursor position
unsigned int disp_getcursor()
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
void disp_clearscreen()
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
	disp_setcursor_o(0);
}

//Prints the hex representation of the short passed
void disp_phex16(unsigned short x)
{
	for(int i = 12; i >= 0; i-=4)
	{
		unsigned char d = (x >> i) & 0x0F;
		d += '0';
		if(d > '9')
			d += 7;
		disp_printc(d);
	}
}

//Prints the hex representation of the integer passed
void disp_phex32(unsigned int x)
{
	for(int i = 28; i >= 0; i-=4)
	{
		unsigned char d = (x >> i) & 0x0F;
		d += '0';
		if(d > '9')
			d += 7;
		disp_printc(d);
	}
}

//Prints the hex representation of the byte passed
void disp_phex8(unsigned char x)
{
	for(int i = 4; i >= 0; i-=4)
	{
		unsigned char d = (x >> i) & 0x0F;
		d += '0';
		if(d > '9')
			d += 7;
		disp_printc(d);
	}
}

void disp_pnum(long x)
{
	if(x < 0)
	{
		disp_printc('-');
		x *= -1;
	}
	
	if(x > 9)
		disp_pnum(x / 10);
	
	disp_printc('0' + (x % 10));
}

//Clears the location under the cursor
void disp_clearcursor()
{
	int offset = disp_getcursor();
	unsigned char* vMemLoc = (unsigned char*)(VMEM_LOCATION);
	vMemLoc[offset] = 0;
	vMemLoc[offset+1] = 0x0F;
}