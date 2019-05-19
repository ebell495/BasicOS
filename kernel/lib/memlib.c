#include "memlib.h"
#include "memlib_ext.h"
#include "display.h"
#define E820_LOC 0x10000
#define E820_COUNT_LOC 0xFFFD
#define K_PAGE_SIZE 64

unsigned int lowMemSize = 0x0;
unsigned int highMemLocation = 0x0;
unsigned int highMemSize = 0x0;

unsigned int bucketStartLocation = 0x0;

void* memcpy(void* restrict dstptr, const void* restrict srcptr, int size) 
{
	unsigned char* dst = ((unsigned char*) dstptr);
	const unsigned char* src = ((const unsigned char*) srcptr);
	for (int i = 0; i < size; i++)
	{
		*dst = *src;
		dst++;
		src++;
	}
	return dstptr;
}

//Kernel memory allocation

void* kmalloc(unsigned int size)
{
	int numPages = (size / K_PAGE_SIZE) + 1;
	void* ret = (void*)bucketStartLocation;
	bucketStartLocation += numPages;
	
	return ret;
}

void kfree(void* pointer)
{
	return;
}

//Reads the e820 map that was loaded during the boot sequence
//It then take the important information out of it and stores the sizes above

void mem_read_e820()
{
	int count = *((int*)(E820_COUNT_LOC));
	
	for(int i = 0; i < count; i++)
	{
		unsigned int baseAddress = 0x0;
		for(int j = 0; j < 4; j++)
		{
			baseAddress += (((unsigned char) *((int*)(E820_LOC + (24*i) + j))) << (j*8));
		}
		
		unsigned int size = 0x0;
		for(int j = 0; j < 4; j++)
		{
			size += ((unsigned char) *((int*)(E820_LOC + (24*i) + (8) + j))) << (j*8);
		}
		
		unsigned char type = (unsigned char) *((int*)(E820_LOC + (24*i) + (16)));
		
		/*
		disp_printc('\n');
		disp_phex8(i);
		disp_printc(' ');
		disp_phex32(baseAddress);
		disp_printc(' ');
		disp_phex32(size);
		disp_printc(' ');
		disp_phex8(type);
		*/
		
		if(type == 1)
		{
			//See if we have found the upper memory location
			if(baseAddress > 0x9FC00)
			{
				highMemLocation = baseAddress;
				highMemSize = size;
				
				bucketStartLocation = baseAddress + size - 16 - (16 - ((baseAddress + size) % 16));
				//nextBucketLocation = bucketStartLocation;
				//nextPageLocation = bucketStartLocation - (sizeof(struct kbucket) * ((size / 4096)));
				/*
				disp_phex32(bucketStartLocation);
				disp_printc('\n');
				disp_phex32(nextPageLocation);
				disp_printc('\n');
				disp_phex32(baseAddress);
				disp_printc('\n');
				disp_phex32(size);
				disp_printc('\n');
				*/
			}
			else
			{
				lowMemSize = size;
			}
		}
	}
}