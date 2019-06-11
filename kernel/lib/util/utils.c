#include "utils.h"
#include "timer.h"
#include "memlib.h"
#include "../drv/display.h"
#include "../drv/hwio.h"

unsigned long long state[16];
unsigned int index = 20;

void seed()
{
	unsigned int seed = time_getsysticks() + time_getCMOSTime(); 
	for(int i = 0; i < 16; i++)
	{
		state[i] = (seed + 1) * ((seed+1) << 2) * i;
	}
}

unsigned int util_genRand()
{
	if(index == 20)
	{
		seed();
		index = 0;
	}

	unsigned long long a, b, c, d; 
    a = state[index]; 
    c = state[(index+13)&15]; 
    b = a^c^(a<<16)^(c<<15); 
    c = state[(index+9)&15]; 
    c ^= (c>>11); 
    a = state[index] = b^c; 
    d = a^((a<<5)&0xDA442D24); 
    index = (index + 15)&15; 
    a = state[index]; 
    state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28); 
    return (int)state[index]; 
}

unsigned char* util_genUUID()
{
	unsigned char* ret = kmalloc(16);
	//unsigned char ret[16];

	unsigned long long rand = util_genRand();

	for(int i = 0; i < 4; i++)
		ret[i] = (rand >> 8*i) & 0xff;

	rand = util_genRand();

	for(int i = 0; i < 4; i++)
		ret[i+4] = (rand >> 8*i) & 0xff;

	rand = util_genRand();

	for(int i = 0; i < 4; i++)
		ret[i+8] = (rand >> 8*i) & 0xff;

	rand = util_genRand();

	for(int i = 0; i < 4; i++)
		ret[i+12] = (rand >> 8*i) & 0xff;

	return ret;
}

char** strsplt(char* str, char token, int* count)
{
	if(str == 0 || *str == 0)
		return 0;

	int len = strlen(str);

	for(int i = 0; i < len; i++)
	{
		if(str[i] == token)
		{
			str[i] = '\0';
			(*count)++;
		}
	}
	(*count)++;

	char** ret = kmalloc(((*count)+1) * sizeof(char**));
	int cc = 0;
	int j = 0;

	for(int i = 0; i < (*count)+1; i++)
	{
		j = 0;
		ret[i] = kmalloc(strlen((str+cc))+1);
		while(str[cc] != '\0')
		{
			ret[i][j] = str[cc];
			j++;
			cc++;
		}
		ret[i][j+1] = '\0';
		cc++;
	}

	ret[(*count)+1] = 0;

	return ret;
}

char strcmp(char* str1, char* str2)
{
	while(*str1 && *str2)
	{
		if(*str1++ != *str2)
			return 1;
	}

	if(*str1 || *str2)
		return 1;

	return 0;
}

unsigned int strlen(char* str)
{
	unsigned int len = 0;
	while(str[len])
		len++;

	return len;
}