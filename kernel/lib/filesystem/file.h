#ifndef _fileh
#define _fileh

#include "LEAN.h"

typedef struct file
{
	struct Inode* inode;
} File;


File* createFile(char* path);
File* createFileWAlloc(char* path, unsigned int preAlloc);

File* openFile(char* path);

void readFileBytes(File* file, unsigned int offset, unsigned char* buffer, unsigned int size);

unsigned char writeFile(File* file, unsigned char* data, unsigned int offset, unsigned int dataSize);

unsigned char* readFile(File* file, unsigned int offset, unsigned int dataSize);

void closeFile(File* file);

void deleteFileWFile(File* file);
void deleteFileWPath(char* path);

void createDirectory(char* path);
void deleteDirectory(char* path);

void createDirectoriesForFile(char* path);

File** getFilesInPath(char* path, unsigned int* numFile);

#endif