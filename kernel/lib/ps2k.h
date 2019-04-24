#ifndef _ps2kh
#define _ps2kh
unsigned char ps2_getkeypress();
unsigned char ps2_waitforkeypress();
unsigned char ps2_getchar(unsigned char scanCode, unsigned char shift);
unsigned char ps2_getscancode();
#endif