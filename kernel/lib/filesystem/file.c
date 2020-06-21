#include "file.h"
#include "../util/utils.h"
#include "../util/memlib.h"
#include "../drv/display.h"
#include "../drv/ata.h"
#include "../drv/hwio.h"

File* createFile(char* path)
{
	File* file = kmalloc(sizeof(File));

	struct Inode* fileNode = LEAN_createInode(iaSTD_FILE);

	int numOfDir = 0;
	char** spPath = strsplt(path, '/', &numOfDir);
	struct DirectoryEntry* cDir = 0;

	unsigned long long cNode = LEAN_getCurrentSuperblock()->rootInode;
	unsigned long long pNode = LEAN_getCurrentSuperblock()->rootInode;

	for(int i = 0; i < numOfDir; i++)
	{
		cDir = LEAN_findDirectoryEntry(cNode, spPath[i]);

		if(cDir->inode == 0 && i == numOfDir-1)
		{
			cNode = fileNode->extentStarts[0];

			kfree(cDir->name);
			kfree(cDir);

			cDir = LEAN_createDirectoryEntry(cNode, FT_REGULAR, spPath[i], 0);
			LEAN_writeDirectoryEntry(pNode, cDir);
		}
		else if(cDir->inode == 0)
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

	file->inode = fileNode;


	return file;
}

File* createFileWAlloc(char* path, unsigned int preAlloc)
{
	return (void*)0;
}

File* openFile(char* path)
{
	struct DirectoryEntry* endOfPath = LEAN_findEndOfPath(path);

	p_serial_printf("End of Path, %s\n", endOfPath->name);

	File* ret = (File*) kmalloc(sizeof(File));

	ret->inode = LEAN_readNode(endOfPath->inode);

	return ret;
}

void readFileBytes(File* file, unsigned int offset, unsigned char* buffer, unsigned int size)
{
	unsigned long long fileStart = file->inode->extentStarts[0] + 1;

	unsigned char* retBuffer = ata_readbytes(fileStart, offset, size);
	memcpy(buffer, retBuffer, size);
	kfree(retBuffer);
}

void closeFile(File* file)
{
	kfree(file->inode);
	kfree(file);
}

void deleteFileWFile(File* file)
{

}

void deleteFileWPath(char* path)
{

}

unsigned char writeFile(File* file, unsigned char* data, unsigned int offset, unsigned int dataSize)
{
	/*unsigned int sectorOffset = offset / 512;
	unsigned int sectorPOffset = offset % 512;
	unsigned int numSectors = (dataSize / 512) + ((dataSize % 512 != 0) ? 1 : 0);

	unsigned int numSectorP = dataSize % 512;

	unsigned int cSectorCount = 0;

	if(sectorOffset > file->inode->sectorCount)
	{

	}
	else
	{
		if(sectorOffset + numSectors > file->inode->sectorCount)
		{

		}
		else
		{
			for(int i = 0; i < file->inode->extentCount; i++)
			{
				for(int j = file->inode->extentStarts[i] + ((i==0) ? 1 : 0); j < file->inode->extentSizes[i]; j++)
				{
					if(sectorOffset >= cSectorCount)
					{
						if(sectorPOffset != 0)
						{
							if(numSectors == 1)
							{
								ata_writebytes(j, sectorPOffset, (data + ((numSectors - 1) * 512)), dataSize);
								return;
							}
							ata_writebytes(j, sectorPOffset, data, 512 - sectorPOffset);

							dataSize -= 512 - sectorPOffset;
							sectorPOffset = 0;
							numSectors++;
						}
						else
						{
							ata_writesector(j, (data + ((numSectors - 1) * 512)));
							dataSize -= 512;
							numSectors++;
						}
					}


					cSectorCount++;
				}
			}
		}
	}*/

	return 0;

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