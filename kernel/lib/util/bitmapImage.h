#ifndef _BITMAPIMAGEh
#define	_BITMAPIMAGEh

#include "../filesystem/file.h"
#include "../drv/hwio.h"
#include "memlib.h"

struct BitmapImage
{
	File* file;
	unsigned int dataOffset;
	unsigned char* imageBuffer;
	unsigned int bufferSize;
	unsigned int imageWidth;
	unsigned int imageHeight;
};

struct BitmapImage loadImage(char* path);
void closeImage(struct BitmapImage image);

//Reformats the image to be packed, with no offset
void reformatImage(struct BitmapImage* image);

#endif