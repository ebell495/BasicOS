#include "LEAN.h"
#include "../drv/ata.h"
#include "../util/memlib.h"
#include "../drv/display.h"
#include "../drv/hwio.h"
#include "../util/utils.h"

struct Superblock* currentSb = 0;

unsigned int LEAN_computeChecksum(const void* data, unsigned short size)
{
	unsigned int res = 0;

	const unsigned int* d = (unsigned int*) (data);
	size /= sizeof(unsigned int);

	for (unsigned short i = 1; i != size; ++i) 
	{
		res = (res << 31) + (res >> 1) + d[i];
	}

	return res;
}

struct Superblock* LEAN_getCurrentSuperblock()
{
	return currentSb;
}

unsigned char LEAN_checkBitmap(unsigned long sector)
{
	unsigned int byte = 0;
	unsigned char bit = 0;

	byte = sector / 8;
	bit = sector % 8;

	unsigned char* bMap = ata_readbytes(currentSb->bitmapStart, byte, 1);

	unsigned char res = ((bMap[0] & (1 << bit)) > 0);

	kfree(bMap);

	return res;
}

void LEAN_markBitmap(unsigned long sector, unsigned char status)
{
	unsigned int byte = 0;
	unsigned char bit = 0;

	byte = sector / 8;
	bit = sector % 8;

	unsigned char* bMap = ata_readbytes(currentSb->bitmapStart, byte, 1);

	if(status == 0)
		bMap[0] = bMap[0] ^ (0 << bit);
	else
		bMap[0] = bMap[0] | (1 << bit);

	ata_writebytes(currentSb->bitmapStart, byte, bMap, 1);

	kfree(bMap);
}

struct Superblock* LEAN_readSuperblock()
{
	unsigned char* mbrSector = ata_readsector(0);
	struct PartitionTableEntry* pt1 = kmalloc(16);

	memcpy(pt1, (void*)(mbrSector + 446), 16);

	if(pt1->status != 0x80 || pt1-> partitionType != 0xEA)
	{
		disp_printstring("Error with partition table: status and partition type were: \n");
		disp_phex8(pt1->status);
		disp_printc(' ');
		disp_phex8(pt1->partitionType);
		return (struct Superblock*) 0;
	}

	unsigned int sbLBA = pt1->lbaStart;

	unsigned char* sbSector = ata_readsector(sbLBA);
	struct Superblock* sb = kmalloc(512);
	memcpy(sb, sbSector, 512);

	if(sb->magic != 0x4E41454C || sb->primarySuper != sbLBA)
	{
		disp_printstring("Error with superblock, magic or superblock sector did not match, magic, primarySuper and readSector were: \n");
		disp_phex32(sb->magic);
		disp_printc(' ');
		disp_phex32(sb->primarySuper);
		disp_printc(' ');
		disp_phex32(sbLBA);

		return (struct Superblock*) 0;
	}

	unsigned int calcChecksum = LEAN_computeChecksum(sb, 512);

	if(sb->checksum != calcChecksum)
	{
		disp_printstring("Error with superblock, checksums did not match. Checksum from block and calculated one was: \n");
		disp_phex32(sb->checksum);
		disp_printc(' ');
		disp_phex32(calcChecksum);
		return (struct Superblock*) 0;
	}

	disp_printstring("Superblock read successfuly");

	kfree(mbrSector);
	kfree(sbSector);
	kfree(pt1);

	return sb;
}

void LEAN_commitSuperblock(struct Superblock* sb)
{
	unsigned int calcChecksum = LEAN_computeChecksum(sb, 512);
	sb->checksum = calcChecksum;

	ata_writesector(sb->primarySuper, (unsigned char*) sb);
	ata_writesector(sb->backupSuper, (unsigned char*) sb);
}

struct Inode* LEAN_readNode(unsigned long nodeNumber)
{
	struct Inode* ret = (struct Inode*)ata_readbytes(nodeNumber, 0, 176);

	unsigned int checksum = LEAN_computeChecksum(ret, 176);

	if(ret->checksum != checksum)
	{
		disp_printstring("Error reading node: checksum does not match: node read: ");
		disp_pnum(nodeNumber);
	}

	return ret;
}

void LEAN_writeInode(struct Inode* node)
{
	node->checksum = LEAN_computeChecksum(node, 176);
	unsigned int location = node->extentStarts[0];
	ata_writesector(location, (unsigned char*) node);

	if(LEAN_checkBitmap(location) == 0)
	{
		for(int i = 0; i < node->sectorCount; i++)
			LEAN_markBitmap(location + i, 1);

		currentSb->freeSectorCount -= node->sectorCount;
		LEAN_commitSuperblock(currentSb);
	}
}

struct Indirect* LEAN_readIndirect(unsigned long nodeNumber)
{
	struct Indirect* ret = (struct Indirect*)ata_readsector(nodeNumber);

	unsigned int checksum = LEAN_computeChecksum(ret, 512);

	if(ret->checksum != checksum)
	{
		disp_printstring("Error reading node: checksum does not match: node read: ");
		disp_pnum(nodeNumber);
	}

	return ret;
}

void LEAN_writeIndirect(struct Indirect* indirect)
{
	indirect->checksum = LEAN_computeChecksum(indirect, 176);
	unsigned int location = indirect->extentStarts[0];
	ata_writesector(location, (unsigned char*) indirect);

	if(LEAN_checkBitmap(location) == 0)
	{
		for(int i = 0; i < indirect->sectorCount; i++)
			LEAN_markBitmap(location + i, 1);

		currentSb->freeSectorCount -= indirect->sectorCount;
		LEAN_commitSuperblock(currentSb);
	}
}

unsigned int LEAN_findNewPartitionLocation(unsigned char partitionNumber, unsigned int size)
{
	unsigned char* mbrSector = ata_readsector(0);
	struct PartitionTableEntry* pt0 = kmalloc(16);
	struct PartitionTableEntry* pt1 = kmalloc(16);
	struct PartitionTableEntry* pt2 = kmalloc(16);
	struct PartitionTableEntry* pt3 = kmalloc(16);

	memcpy(pt0, (void*)(mbrSector + 446 + 0 ), 16);
	memcpy(pt1, (void*)(mbrSector + 446 + 16), 16);
	memcpy(pt2, (void*)(mbrSector + 446 + 32), 16);
	memcpy(pt3, (void*)(mbrSector + 446 + 48), 16);

	unsigned int partitionStart = 0;
	unsigned char foundStart = 0;

	if(pt0->status == 0x80)
	{
		if(partitionNumber == 0)
		{
			partitionStart = pt0->lbaStart;
			pt0->numOfSectors = size;
			pt0->partitionType = 0xEA;
			foundStart = 1;
		}
		else
		{
			partitionStart = pt0->lbaStart + pt0->numOfSectors + 1;
		}
	}
	else
	{
		partitionNumber = 0;
		pt0->lbaStart = 3;
		partitionStart = pt0->lbaStart;
		pt0->numOfSectors = size;
		pt0->partitionType = 0xEA;
		pt0->status = 0x80;
		foundStart = 1;
	}

	if(pt1->status == 0x80 && !foundStart)
	{
		if(partitionNumber == 1)
		{
			partitionStart = pt1->lbaStart;
			pt1->numOfSectors = size;
			pt1->partitionType = 0xEA;
			foundStart = 1;
		}
		else if(partitionNumber > 1)
		{
			partitionStart = pt1->lbaStart + pt1->numOfSectors + 1;
		}
	}
	else if(foundStart==0)
	{
		partitionNumber = 1;
		pt1->lbaStart = partitionStart;
		pt1->numOfSectors = size;
		pt1->partitionType = 0xEA;
		foundStart = 1;
		pt1->status = 0x80;
	}

	if(pt2->status == 0x80 && !foundStart)
	{
		if(partitionNumber == 2)
		{
			partitionStart = pt2->lbaStart;
			pt2->numOfSectors = size;
			pt2->partitionType = 0xEA;
			foundStart = 1;
		}
		else if(partitionNumber > 2)
		{
			partitionStart = pt2->lbaStart + pt2->numOfSectors + 1;
		}
	}
	else if(foundStart == 0)
	{
		partitionNumber = 2;
		pt2->lbaStart = partitionStart;
		pt2->numOfSectors = size;
		pt2->partitionType = 0xEA;
		pt2->status = 0x80;
		foundStart = 1;
	}

	if(pt3->status == 0x80 && !foundStart)
	{
		if(partitionNumber == 3)
		{
			partitionStart = pt3->lbaStart;
			pt3->numOfSectors = size;
			pt3->partitionType = 0xEA;
			foundStart = 1;
		}
	}
	
	if(!foundStart)
	{
		partitionNumber = 3;
		pt3->lbaStart = partitionStart;
		pt3->numOfSectors = size;
		pt3->partitionType = 0xEA;
		pt3->status = 0x80;
		foundStart = 1;
	}

	memcpy((void*)(mbrSector + 446), pt0, 16);
	memcpy((void*)(mbrSector + 446 + 16), pt1, 16);
	memcpy((void*)(mbrSector + 446 + 32), pt2, 16);
	memcpy((void*)(mbrSector + 446 + 48), pt3, 16);
	ata_writesector(0, mbrSector);

	kfree(mbrSector);
	kfree(pt0);
	kfree(pt1);
	kfree(pt2);
	kfree(pt3);


	return partitionStart;
}

struct Superblock* LEAN_createLEANPartition(unsigned char partitionNumber, char* name, unsigned int size)
{
	unsigned int sbLocation = LEAN_findNewPartitionLocation(partitionNumber, size);

	struct Superblock* sb = kmalloc(512);

	sb->magic = 0x4E41454C;
	sb->fsVersion = 0x0006;
	sb->preallocCount = 2;
	sb->logSectorsPerBand = 12;
	sb->state = 0x01;

	unsigned char* uid = util_genUUID();
	for(int i = 0; i < 16; i++)
	{
		sb->uuid[i] = uid[i];
	}

	unsigned char nameLoc = 0;
	do
	{
		sb->volumeLabel[nameLoc] = name[nameLoc];
		nameLoc++;
	}
	while(name[nameLoc] != 0);

	sb->sectorCount = size;
	sb->freeSectorCount = size - 3 - 3;
	sb->primarySuper = sbLocation;
	sb->backupSuper = size-1;
	sb->bitmapStart = sbLocation + 1;
	sb->rootInode = sbLocation + 2;

	LEAN_commitSuperblock(sb);
	currentSb = sb;

	if(partitionNumber == 0)
	{
		LEAN_markBitmap(0, 1);
		LEAN_markBitmap(1, 1);
		LEAN_markBitmap(2, 1);
	}

	LEAN_markBitmap(sbLocation, 1);
	LEAN_markBitmap(size - 1, 1);
	LEAN_markBitmap(sbLocation + 1, 1);

	struct Inode* rootNode = kmalloc(176);

	rootNode->magic = 0x45444F4E;
	rootNode->extentCount = 1;
	rootNode->linkCount = 2;
	rootNode->attributes = iaSTD_DIR;
	rootNode->fileSize = 64;
	rootNode->sectorCount = 3;
	rootNode->extentStarts[0] = sbLocation + 2;
	rootNode->extentSizes[0] = 3;

	LEAN_writeInode(rootNode);

	//ata_writebytes(sbLocation + 3, 0, LEAN_createDirectoryEntry(5, FT_DIRECTORY, ".", 0), 16);
	//ata_writebytes(sbLocation + 3, 16, LEAN_createDirectoryEntry(5, FT_DIRECTORY, "..", 0), 16);
	//ata_writebytes(sbLocation + 3, 32, LEAN_createDirectoryEntry(8, FT_REGULAR, "kernel.bin", 0), 32);

	struct DirectoryEntry* de = LEAN_createDirectoryEntry(5, FT_DIRECTORY, ".", 0);
	LEAN_writeDirectoryEntry(rootNode->extentStarts[0], de);
	kfree(de->name);
	kfree(de);

	de = LEAN_createDirectoryEntry(5, FT_DIRECTORY, "..", 0);
	LEAN_writeDirectoryEntry(rootNode->extentStarts[0], de);
	kfree(de->name);
	kfree(de);
	
	struct Inode* kernelFile = kmalloc(176);
	kernelFile->magic = 0x45444F4E;
	kernelFile->extentCount = 1;
	kernelFile->linkCount = 1;
	kernelFile->attributes = iaSYS_FILE;
	kernelFile->fileSize = 32768;
	kernelFile->sectorCount = 65;
	kernelFile->extentStarts[0] = 8;
	kernelFile->extentSizes[0] = 65;

	LEAN_writeInode(kernelFile);

	de = LEAN_createDirectoryEntry(8, FT_REGULAR, "kernel.bin", 0);
	LEAN_writeDirectoryEntry(rootNode->extentStarts[0], de);

	kfree(kernelFile);
	kfree(rootNode);
	kfree(de->name);
	kfree(de);

	return sb;
}

struct DirectoryEntry* LEAN_createDirectoryEntry(unsigned long long inode, unsigned char type, char* name, unsigned short* size)
{
	unsigned short nameLen = 0;
	while(name[nameLen] != 0)
	{
		nameLen++;
	}

	unsigned char s = 16;
	unsigned short sum = nameLen + 12;

	unsigned char recLen = sum / s + (sum % s != 0);

	struct DirectoryEntry* ret = kmalloc(16 * recLen);

	ret->inode = inode;
	ret->type = type;
	ret->recLen = recLen;
	ret->nameLen = nameLen;
	ret->name = kmalloc(nameLen);

	memcpy(ret->name, name, nameLen);

	if(size != 0)
		*size = recLen * 16;

	return ret;
}

unsigned char* LEAN_dirEntryToByteArray(struct DirectoryEntry* directoryEntry)
{
	unsigned char* ret = kmalloc(directoryEntry->recLen*16);
	memcpy(ret, directoryEntry, 12);
	memcpy((void*)(ret + 12), directoryEntry->name, directoryEntry->nameLen);

	return ret;
}

struct DirectoryEntry* LEAN_readDirectoryEntries(unsigned long long inode)
{
	unsigned int dirEntryLoc = inode+1;
	unsigned char* sector = ata_readsector(dirEntryLoc);
	return 0;
}

void LEAN_writeDirectoryEntry(unsigned long long inode, struct DirectoryEntry* directoryEntry)
{
	unsigned int dirEntryLoc = inode+1;
	struct Inode* node = LEAN_readNode(inode);
	
	int count = 0;

	for(int i = 0; i < node->extentCount; i++)
	{
		for(int j = ((i==0) ? 1 : 0); j <= node->extentSizes[i]; j++)
		{
			count = 0;
			dirEntryLoc = node->extentStarts[i] + j;
			unsigned char* sector = ata_readsector(dirEntryLoc);

			while(count < 512)
			{
				struct DirectoryEntry* header = (struct DirectoryEntry*)&sector[count];
				if(header->inode == 0 && header->type == 0)
				{
					unsigned char* dirEntBytes = LEAN_dirEntryToByteArray(directoryEntry);
					ata_writebytes(dirEntryLoc, count, dirEntBytes, directoryEntry->recLen*16);
					kfree(node);
					kfree(sector);
					kfree(dirEntBytes);
					return;
				}
				count += (header->recLen*16);
			}
		}
	}

	dirEntryLoc = LEAN_getNextOpenSector(inode);

	if(dirEntryLoc != 0)
	{
		unsigned char* dirEntBytes = LEAN_dirEntryToByteArray(directoryEntry);
		ata_writebytes(dirEntryLoc, 0, dirEntBytes, directoryEntry->recLen*16);

		kfree(node);
		kfree(dirEntBytes);

		return;
	}


	//Here if the directory wasn't writen then we would have to check to see if there is another sector allocated or make another extent
}

unsigned int LEAN_getNextOpenSector(unsigned long long inode)
{
	struct Inode* node = (struct Inode*)ata_readsector(inode);

	unsigned int cSector = node->extentStarts[node->extentCount-1] + node->extentSizes[node->extentCount-1];
	if(LEAN_checkBitmap(cSector) == 0 && node->indirectCount == 0 && cSector < 4096)
	{
		node->extentSizes[node->extentCount-1] += 1;
		LEAN_markBitmap(cSector, 1);
		currentSb->freeSectorCount -= 1;

		kfree(node);

		return cSector;
	}

	if(node->extentCount < 6 && cSector < 4096)
	{
		node->extentCount++;
		while(LEAN_checkBitmap(cSector) && LEAN_checkBitmap(cSector+1) != 0 && cSector < 4096)
			cSector++;
		node->extentStarts[node->extentCount - 1] = cSector;
		node->extentSizes[node->extentCount - 1] += 2;
		LEAN_markBitmap(cSector, 1);
		LEAN_markBitmap(cSector+1, 1);

		currentSb->freeSectorCount -= 2;

		kfree(node);

		return cSector;
	}


	// if(node->indirectCount == 0)
	// {
	// 	node->indirectCount++;
	// 	while(LEAN_checkBitmap(cSector) && LEAN_checkBitmap(cSector+1) && LEAN_checkBitmap(cSector+2) != 0 && cSector < 4096)
	// 		cSector++;

	// 	struct Indirect Indirect = kmalloc(512);

	// }

	kfree(node);

	return 0;

}