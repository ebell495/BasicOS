#ifndef _BITMAPIMAGEh
#define	_BITMAPIMAGEh

#include "../filesystem/file.h"
#include "../drv/hwio.h"
#include "memlib.h"

//Image structure
//Stores a pointer to the backing file
//The raw image data
//The size of the raw image data
//The lenght and width in pixels of the image
struct BitmapImage
{
	File* file;
	unsigned char* imageBuffer;
	unsigned int bufferSize;
	unsigned int imageWidth;
	unsigned int imageHeight;
};

//Loads a filepath in as an image
struct BitmapImage loadImage(char* path);

//Closes the image
//Releases all memory associated with the image, including the file
void closeImage(struct BitmapImage image);

//Reformats the image to be packed, with no offset
void reformatImage(struct BitmapImage* image);

#endif