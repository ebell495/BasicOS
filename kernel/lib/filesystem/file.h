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
File* createFile(char* path, unsigned int preAlloc);

File* openFile(char* path);
void closeFile(File* file);

void deleteFile(File* file);
void deleteFile(char* path);

void createDirectory(char* path);
void deleteDirectory(char* path);

File** getFilesInPath(char* path, unsigned int* numFile);

void setSuperblock(struct Superblock* sb);

#endif