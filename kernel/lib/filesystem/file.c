#include "file.h"
#include "../util/utils.h"
#include "../util/memlib.h"
#include "../drv/display.h"

File* createFile(char* path)
{
	return (void*)0;
}

File* createFileWAlloc(char* path, unsigned int preAlloc)
{
	return (void*)0;
}

File* openFile(char* path)
{
	return (void*)0;
}

void closeFile(File* file)
{

	kfree(file);
}

void deleteFileWFile(File* file)
{

}

void deleteFileWPath(char* path)
{

}

void createDirectory(char* path)
{
	int numOfDir = 0;
	char** spPath = strsplt(path, '/', &numOfDir);
	struct DirectoryEntry* cDir = 0;

	unsigned long long cNode = LEAN_getCurrentSuperblock()->rootInode;
	unsigned long long pNode = LEAN_getCurrentSuperblock()->rootInode;

	for(int i = 0; i < numOfDir; i++)
	{
		cDir = LEAN_findDirectoryEntry(cNode, spPath[i]);

		if(cDir->inode == 0)
		{
			struct Inode* newNode = LEAN_createInode(iaSTD_DIR);
			cNode = newNode->extentStarts[0];

			kfree(cDir->name);
			kfree(cDir);

			cDir = LEAN_createDirectoryEntry(cNode, FT_DIRECTORY, spPath[i], 0);
			LEAN_writeDirectoryEntry(pNode, cDir);

			struct DirectoryEntry* de = LEAN_createDirectoryEntry(cNode, FT_DIRECTORY, ".", 0);
			LEAN_writeDirectoryEntry(cNode, de);

			kfree(de->name);
			kfree(de);

			de = LEAN_createDirectoryEntry(pNode, FT_DIRECTORY, "..", 0);
			LEAN_writeDirectoryEntry(cNode, de);

			kfree(de->name);
			kfree(de);

			kfree(newNode);
		}

		pNode = cNode;
		cNode = cDir->inode;

		kfree(cDir->name);
		kfree(cDir);
		kfree(spPath[i]);
	}

	kfree(spPath[numOfDir]);
	kfree(spPath);
}

void deleteDirectory(char* path)
{

}

File** getFilesInPath(char* path, unsigned int* numFile)
{
	return (void*)0;
}