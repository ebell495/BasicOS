#ifndef _displayh
#define _displayh

void disp_printc(unsigned char c);
void disp_printstring(char* string);
void disp_setcursor_rc(int row, int col);
void disp_setcursor_o(int offset);
void disp_clearscreen();
void disp_clearcursor();
void disp_backspace();
void disp_scroll();
unsigned int disp_getcursor();
void disp_phex16(unsigned short x);
void disp_phex32(unsigned int x);
void disp_phex8(unsigned char x);
#endif