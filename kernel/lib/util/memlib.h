#ifndef _memlibh
#define _memlibh

void* memcpy(void* restrict dstptr, const void* restrict srcptr, int size);
int memcmp(const void* restrict ptr1, const void* restrict ptr2, unsigned int size);
void* kmalloc(unsigned int size);
void kfree(void* pointer);
unsigned int mem_getUsedMem();
unsigned int mem_getFreeMem();

void mem_read_e820();

#endif