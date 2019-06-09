#ifndef _LEANh
#define _LEANh
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
#define isSTD_FILE			iaUSR_ALL + iaFMT_REGULAR + iaINLINE_EXT_ATTR + iaPREALLOC + iaNO_ACCESS_TIME

struct Superblock
{
	unsigned int checksum;
	unsigned int magic;
	unsigned short fsVersion;
	unsigned char preallocCount;
	unsigned char logSectorsPerBand;
	unsigned int state;
	unsigned char uuid[16];
	char volumeLabel[64];
	unsigned long long sectorCount;
	unsigned long long freeSectorCount;
	unsigned long long primarySuper;
	unsigned long long backupSuper;
	unsigned long long bitmapStart;
	unsigned long long rootInode;
	unsigned long long badInode;

	unsigned char reserved[360];
}__attribute__((packed));

struct Inode
{
	unsigned int checksum;
	unsigned int magic;
	unsigned char extentCount;
	unsigned char reserved[3];
	unsigned int indirectCount;
	unsigned int linkCount;
	unsigned int uid;
	unsigned int gid;
	unsigned int attributes;
	unsigned long long fileSize;
	unsigned long long sectorCount;
	long long accessTime;
	long long statusChangeTime;
	long long modificationTime;
	long long creationTime;
	unsigned long long firstIndirect;
	unsigned long long lastIndirect;
	unsigned long long fork;
	unsigned long long extentStarts[6];
	unsigned int extentSizes[6];
}__attribute__((packed));

struct PartitionTableEntry
{
	unsigned char status;
	unsigned char chsStart[3];
	unsigned char partitionType;
	unsigned char chsEnd[3];
	unsigned int lbaStart;
	unsigned int numOfSectors;
}__attribute__((packed));

struct DirectoryEntry
{
	unsigned long long inode;
	unsigned char type;
	unsigned char recLen;
	unsigned short nameLen;
	char name[];
};

unsigned int computeChecksum(const void* data, unsigned short size);

unsigned char checkBitmap(struct Superblock* sb, unsigned long sector);
void markBitmap(struct Superblock* sb, unsigned long sector, unsigned char status);

struct Superblock* readSuperblock();
void commitSuperblock(struct Superblock* sb);

struct Inode* readNode(struct Superblock* sb, unsigned long nodeNumber);
void writeInode(struct Superblock* sb, struct Inode* node);

struct Superblock* createLEANPartition(unsigned char partitionNumber, char* name, unsigned int size);

unsigned char* createDirectoryEntry(unsigned long long inode, unsigned char type, char* name, unsigned short* size);

#endif