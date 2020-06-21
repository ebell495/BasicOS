#include "bitmapImage.h"

struct BitmapImage loadImage(char* path)
{
	File* imageFile = openFile(path);
	char imageHeader[54];
	readFileBytes(imageFile, 2, imageHeader, 54);
	unsigned int size = *((unsigned int*) imageHeader);
	unsigned int offset = *((unsigned int*) (imageHeader + 8));
	unsigned int imageWidth = *((unsigned int*) (imageHeader + 16));
	unsigned int imageHeight = *((unsigned int*) (imageHeader + 20));
	unsigned int totalImageSize = size - offset;

	p_serial_printf("Width %i, Height %i\n", imageWidth, imageHeight);

	struct BitmapImage ret;
	ret.file = imageFile;
	ret.imageBuffer = kmalloc(totalImageSize);
	ret.bufferSize = totalImageSize;
	ret.imageWidth = imageWidth;
	ret.imageHeight = imageHeight;

	readFileBytes(imageFile, offset, ret.imageBuffer, totalImageSize);
	reformatImage(&ret);

	return ret;
}

void closeImage(struct BitmapImage image)
{
	closeFile(image.file);
	kfree(image.imageBuffer);
}

void reformatImage(struct BitmapImage* image)
{
	unsigned char* newBuf = kmalloc(image->bufferSize);

	unsigned int pixImageWidth = image->imageWidth * 3;
	unsigned char padding = 4 - (pixImageWidth % 4);

	if(padding == 4)
		padding = 0;

	p_serial_printf("pixImageWidth %i, Padding %i\n", pixImageWidth, padding);

	for(int i = 0; i < image->imageHeight; i++)
	{
		for(int j = 0; j < image->imageWidth; j++)
		{
			newBuf[i * pixImageWidth + (j * 3)] = image->imageBuffer[(image->imageHeight - (i + 1)) * (pixImageWidth + padding) + (j * 3)];
			newBuf[i * pixImageWidth + (j * 3) + 1] = image->imageBuffer[(image->imageHeight - (i + 1)) * (pixImageWidth + padding) + (j * 3) + 1];
			newBuf[i * pixImageWidth + (j * 3) + 2] = image->imageBuffer[(image->imageHeight - (i + 1)) * (pixImageWidth + padding) + (j * 3) + 2];
		}
	}

	//memcpy(image->imageBuffer, newBuf, image->bufferSize);

	kfree(image->imageBuffer);
	image->imageBuffer = newBuf;
}