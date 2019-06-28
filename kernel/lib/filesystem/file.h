#ifndef _fileh
#define _fileh

#include "LEAN.h"

typedef struct file
{
	unsigned int directoryEntryNode;
	unsigned int directoryEntryOffset;
	struct Inode* inode;
	struct DirectoryEntry* directoryEntry;
} File;


File* createFile(char* path);
File* createFileWAlloc(char* path, unsigned int preAlloc);

File* openFile(char* path);
void closeFile(File* file);

void deleteFileWFile(File* file);
void deleteFileWPath(char* path);

void createDirectory(char* path);
void deleteDirectory(char* path);

File** getFilesInPath(char* path, unsigned int* numFile);

#endif