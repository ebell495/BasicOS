#include "file.h"
#include "../util/utils.h"

Superblock* sb = 0;

void setSuperblock(struct Superblock* superblock)
{
	sb = superblock;
}

File* createFile(char* path)
{
	int numOfDir = 0;
	char** spPath = strsplt(path, '/', &numOfDir);

	if(numOfDir == 1)
	{

	}

	return (void*)0;
}

File* createFile(char* path, unsigned int preAlloc)
{
	return (void*)0;
}

File* openFile(char* path)
{
	return (void*)0;
}

void closeFile(File* file)
{

}

void deleteFile(File* file)
{

}

void deleteFile(char* path)
{

}

void createDirectory(char* path)
{

}

void deleteDirectory(char* path)
{

}

File** getFilesInPath(char* path, unsigned int* numFile)
{
	return (void*)0;
}