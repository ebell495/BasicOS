#ifndef _memlibh
#define _memlibh

void* memcpy(void* restrict dstptr, const void* restrict srcptr, int size);
int memcmp(const void* restrict ptr1, const void* restrict ptr2, unsigned int size);
void memset(const void* restrict ptr1, unsigned char val, int size);
void* kmalloc(unsigned int size);
void kfree(void* pointer);
void dumpMemLoc(unsigned int loc, unsigned int amount);

unsigned int mem_getUsedMem();
unsigned int mem_getFreeMem();
unsigned int mem_getPeakUse();
unsigned int mem_getMemSize();

unsigned int getBitmapStart();

void mem_read_e820();

#endif