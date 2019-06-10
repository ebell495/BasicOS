#include "memlib.h"
#include "memlib_ext.h"
#include "../drv/display.h"

#define E820_LOC 0x10000
#define E820_COUNT_LOC 0xFFFD
#define K_PAGE_SIZE 520

unsigned int lowMemSize = 0x0;
unsigned int highMemLocation = 0x0;
unsigned int highMemSize = 0x0;

unsigned int bucketStartLocation = 0x0;
unsigned int bitmapStartLocation = 0x0;

unsigned int memInUse = 0;

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

int memcmp(const void* restrict ptr1, const void* restrict ptr2, unsigned int size)
{
	int cmp = 0;
	for(unsigned int i = 0; i < size; i++)
	{
		cmp = ((char*)ptr1)[i] - ((char*)ptr2)[i];
		if(cmp!=0)
			return cmp;
	}

	return 0;
}

void memset(const void* restrict ptr1, unsigned char val, int size)
{
	unsigned char* dst = ((unsigned char*) ptr1);
	for (int i = 0; i < size; i++)
	{
		*dst = val;
		dst++;
	}
}

//Kernel memory allocation

void* kmalloc(unsigned int size)
{
	unsigned int numOfPage = (size+8) / K_PAGE_SIZE + ((size+8) % K_PAGE_SIZE != 0);

	int start = findNumMemLoc(numOfPage);

	start *= K_PAGE_SIZE;

	if(start < 0)
	{
		disp_printstring("Error allocating memory, no space available");
		return 0;
	}
	
	void* ret = (void*)(highMemLocation + start + 8);

	for(int i = 0; i < numOfPage; i++)
	{
		mem_markbitmap(i + (start/K_PAGE_SIZE), 1);
	}

	unsigned int* pt_numPage = (unsigned int*)(highMemLocation + start);
	*pt_numPage = numOfPage;

	pt_numPage = (unsigned int*)(highMemLocation + start + 4);
	*pt_numPage = (start / K_PAGE_SIZE);

	return ret;
}

void kfree(void* pointer)
{
	unsigned int size = *(unsigned int*)(pointer-8);
	unsigned int bmapLoc = *(unsigned int*)(pointer-4);

	for(int i = 0; i < size; i++)
	{
		mem_markbitmap(bmapLoc + i, 0);
	}

	return;
}

unsigned int mem_getUsedMem()
{
	return memInUse;
}

unsigned int mem_getFreeMem()
{
	return bitmapStartLocation -  highMemLocation - mem_getUsedMem();
}

int findNumMemLoc(unsigned int num)
{
	unsigned int start = 0;
	unsigned int maxCon = 0;

	for(int i = 0; i < bitmapStartLocation - highMemLocation; i++)
	{
		if(mem_checkbitmap(i) == 0)
		{
			maxCon += 1;

			if(maxCon == num)
			{
				return start;
			}
		}
		else
		{
			maxCon = 0;
			start = (i+1);
		}


	}
	return -1;
}

void mem_markbitmap(unsigned int loc, unsigned char status)
{
	unsigned int byte = 0;
	unsigned char bit = 0;

	byte = loc / 8;
	bit = loc % 8;

	unsigned char* bMap = (unsigned char*)(bitmapStartLocation + byte);

	if(status == 0)
	{
		bMap[0] = bMap[0] ^ (1 << bit);
		memInUse -= K_PAGE_SIZE;
		memset((void*)(loc*K_PAGE_SIZE+highMemLocation), 0, K_PAGE_SIZE);
	}
	else
	{
		bMap[0] = bMap[0] | (1 << bit);
		memInUse += K_PAGE_SIZE;
	}
}

unsigned char mem_checkbitmap(unsigned int loc)
{
	unsigned int byte = 0;
	unsigned char bit = 0;

	byte = loc / 8;
	bit = loc % 8;

	unsigned char* bMap = (unsigned char*)(bitmapStartLocation + byte);

	return ((bMap[0] & (1 << bit)) > 0);
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
				
				bucketStartLocation = baseAddress;

				bitmapStartLocation = (size + baseAddress) - (size / K_PAGE_SIZE / 8);
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