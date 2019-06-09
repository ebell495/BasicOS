#include "LEAN.h"
#include "ata.h"
#include "memlib.h"
#include "display.h"
#include "hwio.h"

unsigned int computeChecksum(const void* data, unsigned short size)
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

unsigned char checkBitmap(struct Superblock* sb, unsigned long sector)
{
	unsigned int byte = 0;
	unsigned char bit = 0;

	byte = sector / 8;
	bit = sector % 8;

	unsigned char* bMap = ata_readbytes(sb->bitmapStart, byte, 1);

	return bMap[0] & (1 << bit);
}

void markBitmap(struct Superblock* sb, unsigned long sector, unsigned char status)
{
	unsigned int byte = 0;
	unsigned char bit = 0;

	byte = sector / 8;
	bit = sector % 8;

	unsigned char* bMap = ata_readbytes(sb->bitmapStart, byte, 1);

	if(status == 0)
		bMap[0] = bMap[0] & (0 << bit);
	else
		bMap[0] = bMap[0] | (1 << bit);

	ata_writebytes(sb->bitmapStart, byte, bMap, 1);


}

struct Superblock* readSuperblock()
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

	unsigned int calcChecksum = computeChecksum(sb, 512);

	if(sb->checksum != calcChecksum)
	{
		disp_printstring("Error with superblock, checksums did not match. Checksum from block and calculated one was: \n");
		disp_phex32(sb->checksum);
		disp_printc(' ');
		disp_phex32(calcChecksum);
		return (struct Superblock*) 0;
	}

	disp_printstring("Superblock read successfuly");

	return sb;
}

void commitSuperblock(struct Superblock* sb)
{
	unsigned int calcChecksum = computeChecksum(sb, 512);
	sb->checksum = calcChecksum;

	ata_writesector(sb->primarySuper, (unsigned char*) sb);
	ata_writesector(sb->backupSuper, (unsigned char*) sb);
}

struct Inode* readNode(struct Superblock* sb, unsigned long nodeNumber)
{
	struct Inode* ret = (struct Inode*)ata_readbytes(nodeNumber, 0, 176);

	unsigned int checksum = computeChecksum(ret, 176);

	if(ret->checksum != checksum)
	{
		disp_printstring("Error reading node: checksum does not match: node read: ");
		disp_pnum(nodeNumber);
	}

	return ret;
}

void writeInode(struct Superblock* sb, struct Inode* node)
{
	node->checksum = computeChecksum(node, 176);
	unsigned int location = node->extentStarts[0];
	ata_writesector(location, (unsigned char*) node);

	for(int i = 0; i < node->sectorCount; i++)
		markBitmap(sb, location + i, 1);

	sb->freeSectorCount -= node->sectorCount;
	commitSuperblock(sb);
}

unsigned int findNewPartitionLocation(unsigned char partitionNumber, unsigned int size)
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

	return partitionStart;
}

struct Superblock* createLEANPartition(unsigned char partitionNumber, char* name, unsigned int size)
{
	unsigned int sbLocation = findNewPartitionLocation(partitionNumber, size);

	struct Superblock* sb = kmalloc(512);

	sb->magic = 0x4E41454C;
	sb->fsVersion = 0x0006;
	sb->preallocCount = 2;
	sb->logSectorsPerBand = 12;
	sb->state = 0x01;

	unsigned char uid[16] = {0x71,0x56,0xD7,0xC4,0xAD,0xE2,0x43,0x30,0xA9,0x2E,0xCF,0x5C,0x65,0x3A,0xEB,0x48};
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

	commitSuperblock(sb);

	if(partitionNumber == 0)
	{
		markBitmap(sb, 0, 1);
		markBitmap(sb, 1, 1);
		markBitmap(sb, 2, 1);
	}

	markBitmap(sb, sbLocation, 1);
	markBitmap(sb, size - 1, 1);
	markBitmap(sb, sbLocation + 1, 1);

	struct Inode* rootNode = kmalloc(176);

	rootNode->magic = 0x45444F4E;
	rootNode->extentCount = 1;
	rootNode->linkCount = 2;
	rootNode->attributes = iaSTD_DIR;
	rootNode->fileSize = 64;
	rootNode->sectorCount = 3;
	rootNode->extentStarts[0] = sbLocation + 2;
	rootNode->extentSizes[0] = 3;

	writeInode(sb, rootNode);

	ata_writebytes(sbLocation + 3, 0, createDirectoryEntry(5, FT_DIRECTORY, "..", 0), 16);
	ata_writebytes(sbLocation + 3, 16, createDirectoryEntry(5, FT_DIRECTORY, ".", 0), 16);
	ata_writebytes(sbLocation + 3, 32, createDirectoryEntry(8, FT_REGULAR, "kernel.bin", 0), 32);

	
	struct Inode* kernelFile = kmalloc(176);
	kernelFile->magic = 0x45444F4E;
	kernelFile->extentCount = 1;
	kernelFile->linkCount = 1;
	kernelFile->attributes = iaSYS_FILE;
	kernelFile->fileSize = 14848;
	kernelFile->sectorCount = 30;
	kernelFile->extentStarts[0] = 8;
	kernelFile->extentSizes[0] = 30;

	writeInode(sb, kernelFile);

	return sb;
}

unsigned char* createDirectoryEntry(unsigned long long inode, unsigned char type, char* name, unsigned short* size)
{
	unsigned short nameLen = 0;
	while(name[nameLen] != 0)
	{
		nameLen++;
	}

	unsigned char s = 16;
	unsigned short sum = nameLen + 12;

	unsigned char recLen = sum / s + (sum % s != 0);

	unsigned char* ret = kmalloc(16 * recLen);

	memcpy(ret, &inode, 8);
	ret[8] = type;
	ret[9] = recLen;
	memcpy((void*)(ret+10), &nameLen, 2);
	memcpy((void*)(ret+12), name, nameLen);

	if(size != 0)
		*size = recLen * 16;
	return ret;
}