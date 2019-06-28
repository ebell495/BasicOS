//This is the extensions to the memlib
//In here are the extra functions needed to implement a malloc and free
#ifndef _memlib_ext_h
#define _memlib_ext_h
int findNumMemLoc(unsigned int num);

void mem_markbitmap(unsigned int loc, unsigned char status);

unsigned char mem_checkbitmap(unsigned int loc);

#endif