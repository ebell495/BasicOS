#ifndef _LEANh
#define _LEANh

#include <fstream>
#include <iostream>
#include <string>
#include <assert.h>
#include <memory>
#include <cstring>

//Helper Structs
#define iaRUSR 				(1 << 8)
#define iaWUSR 				(1 << 7)
#define iaXUSR 				(1 << 6)
#define iaHIDDEN 			(1 << 12)
#define iaSYSTEM		    (1 << 13)
#define iaARCHIVE 			(1 << 14)
#define iaSYNC 				(1 << 15)
#define iaNO_ACCESS_TIME	(1 << 16)
#define iaIMMUTABLE      	(1 << 17)
#define iaPREALLOC       	(1 << 18)
#define iaINLINE_EXT_ATTR	(1 << 19)
#define iaFMT_MASK       	(7 << 29)
#define iaFMT_REGULAR		(1 << 29)
#define iaFMT_DIRECTORY		(2 << 29)
#define iaFMT_SYMLINK		(3 << 29)
#define iaFMT_FORK			(4 << 29)

#define FT_NONE				(0)
#define FT_REGULAR	  		(iaFMT_REGULAR >> 29)
#define FT_DIRECTORY	  	(iaFMT_DIRECTORY >> 29)
#define FT_SYMLINK	  		(iaFMT_SYMLINK >> 29)

#define iaUSR_ALL			iaRUSR + iaWUSR + iaXUSR
#define iaSYS_DIR			iaUSR_ALL + iaFMT_DIRECTORY + iaINLINE_EXT_ATTR + iaPREALLOC + iaNO_ACCESS_TIME + iaSYSTEM
#define iaSTD_DIR			iaUSR_ALL + iaFMT_DIRECTORY + iaINLINE_EXT_ATTR + iaPREALLOC + iaNO_ACCESS_TIME
#define iaSYS_FILE			iaUSR_ALL + iaFMT_REGULAR + iaINLINE_EXT_ATTR + iaPREALLOC + iaNO_ACCESS_TIME + iaSYSTEM
#define iaSTD_FILE			iaUSR_ALL + iaFMT_REGULAR + iaINLINE_EXT_ATTR + iaPREALLOC + iaNO_ACCESS_TIME

#define EXTENTS_PER_INODE 6
#define EXTENTS_PER_INDIRECT 38

#define bitmap_set_bit(bitmap, bit) bitmap[bit / 8] = bitmap[bit / 8] | (1 << (bit % 8))
#define bitmap_clear_bit(bitmap, bit) bitmap[bit / 8] = (((bitmap[bit / 8] & ((1 << (bit % 8)))) > 0) ? (bitmap[bit / 8] ^ (1 << (bit % 8))) : bitmap[bit / 8])
#define bitmap_is_bit_set(bitmap, bit) ((bitmap[bit / 8] & (1 << (bit % 8))) > 0)


typedef struct Superblock
{
	uint32_t checksum;
	uint32_t magic;
	uint16_t fsVersion;
	uint8_t preallocCount;
	uint8_t logSectorsPerBand;
	uint32_t state;
	uint8_t uuid[16];
	char volumeLabel[64];
	uint64_t sectorCount;
	uint64_t freeSectorCount;
	uint64_t primarySuper;
	uint64_t backupSuper;
	uint64_t bitmapStart;
	uint64_t rootInode;
	uint64_t badInode;

	uint8_t reserved[360];
} Superblock;

typedef struct Inode
{
	uint32_t checksum;
	uint32_t magic;
	uint8_t extentCount;
	uint8_t reserved[3];
	uint32_t indirectCount;
	uint32_t linkCount;
	uint32_t uid;
	uint32_t gid;
	uint32_t attributes;
	uint64_t fileSize;
	uint64_t sectorCount;
	int64_t accessTime;
	int64_t statusChangeTime;
	int64_t modificationTime;
	int64_t creationTime;
	uint64_t firstIndirect;
	uint64_t lastIndirect;
	uint64_t fork;
	uint64_t extentStarts[EXTENTS_PER_INODE];
	uint32_t extentSizes[EXTENTS_PER_INODE];

	uint8_t inlineAttributes[336];
} Inode;

typedef struct Indirect
{
	uint32_t checksum;
	uint32_t magic;
	uint64_t sectorCount;
	uint64_t inode;
	uint64_t thisSector;
	uint64_t prevIndirect;
	uint64_t nextIndirect;
	uint8_t extentCount;
	uint8_t reserved1[3];
	uint32_t reserved2;
	uint64_t extentStarts[EXTENTS_PER_INDIRECT];
	uint32_t extentSizes[EXTENTS_PER_INDIRECT];
} Indirect;

typedef struct DirectoryEntry
{
	uint64_t inode;
	uint8_t type;
	uint8_t recLen;
	uint16_t nameLen;
	std::shared_ptr<char> name;
} DirectoryEntry;

class LeanFS
{
public:
	LeanFS(std::string filename);
	virtual ~LeanFS();

	int mountfs();
	int umountfs();
	int mkfs(uint8_t preallocCount, uint8_t logSectorsPerBand, uint8_t superblockLoc, uint32_t size, std::string volumeLabel);

	int mkdir(std::string path);
	int addFile(std::string file, uint8_t* data, uint32_t size, bool doPathFollow);

	int directWrite(uint8_t* data, uint32_t size, uint32_t sector);
private:
	std::fstream fsFile;
	Superblock* superblock = 0;
	uint8_t* bitmap = 0;

	uint32_t computeChecksum(const void* data, size_t size);

	void makeSuperblock(uint8_t preallocCount, uint8_t logSectorsPerBand, uint8_t superblockLoc, uint32_t size, std::string volumeLabel);
	void commitSuperblock();

	Inode* makeInode(uint32_t attribs, uint32_t parentNode);
	Inode* makeInode(uint32_t attribs, uint32_t size, uint32_t parentNode);
	Inode* findInode(uint32_t inodeAddress);
	void commitInode(Inode* inode);

	DirectoryEntry* createDirectoryEntry(Inode* linkedInode, uint8_t type, std::string name);
	DirectoryEntry* findDirectoryEntry(Inode* inode, std::string name);
	void commitDirectoryEntry(Inode* parentInode, DirectoryEntry* directoryEntry);

	void markBitmap(uint32_t sector, uint8_t status);
	uint8_t checkBitmap(uint32_t sector);
	void commitBitmap();

	uint32_t findNextLocation(uint32_t size, uint32_t currentLocation);
};


#endif