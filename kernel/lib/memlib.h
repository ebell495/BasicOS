#ifndef _memlibh
#define _memlibh

void* memcpy(void* restrict dstptr, const void* restrict srcptr, int size);
void* kmalloc(unsigned int size);
void mem_read_e820();

#endif