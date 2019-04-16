#include "memlib.h"

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