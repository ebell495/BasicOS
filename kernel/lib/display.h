#ifndef _displayh
#define _displayh

void printc(unsigned char c);
void setcursor_rc(int row, int col);
void setcursor_o(int offset);
void clearscreen();
void clearcursor();
unsigned int getcursor();
void pHex16(unsigned short x);
void pHex32(unsigned int x);
void pHex8(unsigned char x);
#endif