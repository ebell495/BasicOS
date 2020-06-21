#include "LEAN.h"
#include <stdlib.h>
#include <time.h>
#include <memory>
#include <sstream>

LeanFS::LeanFS(std::string filename)
{
	this->fsFile.open(filename, std::fstream::binary | std::fstream::out);
	this->fsFile.write("", 1);
	this->fsFile.close();
	this->fsFile.open(filename, std::fstream::binary | std::fstream::out | std::fstream::in);
}

LeanFS::~LeanFS()
{
	this->umountfs();

	this->fsFile.close();
}

int LeanFS::mountfs()
{
	return 0;
}

int LeanFS::umountfs()
{
	if (this->superblock)
	{
		this->superblock->state = 0x01;
		this->commitBitmap();
		this->commitSuperblock();

		delete this->superblock;
		delete this->bitmap;

		this->superblock = 0;
		this->bitmap = 0;
	}
	return 0;
}

int LeanFS::mkfs(uint8_t preallocCount, uint8_t logSectorsPerBand, uint8_t superblockLoc, uint32_t size, std::string volumeLabel)
{
	this->makeSuperblock(preallocCount, logSectorsPerBand, superblockLoc, size, volumeLabel);
	this->commitSuperblock();

	return 0;
}

int LeanFS::mkdir(std::string path)
{
	std::stringstream filenameStream(path);

	std::string tmp;

	Inode* inode = this->findInode(this->superblock->rootInode);

	while (std::getline(filenameStream, tmp, '/'))
	{
		Inode* newNode = this->makeInode(iaSTD_DIR, inode->extentStarts[0]);

		DirectoryEntry* de = this->createDirectoryEntry(newNode, FT_DIRECTORY, tmp);
		this->commitDirectoryEntry(inode, de);
		delete de;

		de = this->createDirectoryEntry(inode, FT_DIRECTORY, "..");
		this->commitDirectoryEntry(newNode, de);
		delete de;

		de = this->createDirectoryEntry(newNode, FT_DIRECTORY, ".");
		this->commitDirectoryEntry(newNode, de);
		delete de;

		delete inode;

		inode = newNode;
	}

	return 0;
}

int LeanFS::addFile(std::string file, uint8_t* data, uint32_t size, bool doPathFollow)
{
	std::stringstream filenameStream(file);

	int endCount = file.find_last_of('/');

	std::string pathString;
	std::string filename;
	std::string tmp;

	Inode* inode = NULL;
	inode = this->findInode(this->superblock->rootInode);
	std::cout << inode->extentStarts[0] << " " << this->superblock->rootInode << " " << endCount << std::endl;

	while (std::getline(filenameStream, tmp, '/') || (endCount < 0))
	{
		if (pathString.length() >= endCount - 2 || endCount < 0)
		{
			filename.append(tmp);
			std::cout << inode->extentStarts[0] << " " << this->superblock->rootInode << " " << endCount << std::endl;

			Inode* newNode = this->makeInode(iaSTD_FILE, size/512, inode->extentStarts[0]);
			DirectoryEntry* de = this->createDirectoryEntry(newNode, FT_REGULAR, tmp);
			std::cout << de->name << " " << de->inode << std::endl;
			this->commitDirectoryEntry(inode, de);
			delete de;

			this->fsFile.seekp((newNode->extentStarts[0] + 1) * 512);
			this->fsFile.write((char*)data, size);

			delete inode;
			delete newNode;

			return 0;
		}
		else
		{
			pathString.append(tmp);
			pathString.append("/");
			if (doPathFollow)
			{
				Inode* newNode = this->makeInode(iaSTD_DIR, inode->extentStarts[0]);

				//Check if a directory entry already exists
				DirectoryEntry* de = this->createDirectoryEntry(newNode, FT_DIRECTORY, tmp);
				this->commitDirectoryEntry(inode, de);
				delete de;

				de = this->createDirectoryEntry(inode, FT_DIRECTORY, "..");
				this->commitDirectoryEntry(newNode, de);
				delete de;

				de = this->createDirectoryEntry(newNode, FT_DIRECTORY, ".");
				this->commitDirectoryEntry(newNode, de);
				delete de;

				delete inode;

				inode = newNode;

				DirectoryEntry* t = this->findDirectoryEntry(newNode, ".");
				if (t == 0)
				{
					std::cout << "Error reading directory entry for " << pathString << std::endl;
					exit(1);
				}

				std::cout << t->inode << " " << t->recLen << " " << t->nameLen << " " << t->name << std::endl;
			}
		}
	}



	return 0;
}

int LeanFS::directWrite(uint8_t* data, uint32_t size, uint32_t sector)
{
	this->fsFile.seekp(sector * 512);
	this->fsFile.write((char*)data, size);

	return 0;
}

uint32_t LeanFS::computeChecksum(const void* data, size_t size)
{
	uint32_t res = 0;
	const uint32_t* d = static_cast<const uint32_t*>(data);

	assert((size & (sizeof(uint32_t) - 1)) == 0);

	size /= sizeof(uint32_t);

	for (size_t i = 1; i != size; ++i) 
		res = (res << 31) + (res >> 1) + d[i];
	return res;
}

void LeanFS::makeSuperblock(uint8_t preallocCount, uint8_t logSectorsPerBand, uint8_t superblockLoc, uint32_t size, std::string volumeLabel)
{
	Superblock* ret = new Superblock();
	srand((unsigned)time(NULL));

	assert(superblockLoc < 32);

	ret->magic = 0x4E41454C;
	ret->fsVersion = 0x0006;
	ret->preallocCount = preallocCount;
	ret->logSectorsPerBand = logSectorsPerBand;
	ret->state = 0x00;
	ret->sectorCount = size;
	ret->freeSectorCount = (uint64_t)((uint64_t)size - ((uint64_t)superblockLoc + 2));
	ret->primarySuper = superblockLoc;
	ret->backupSuper = size - 1;
	ret->bitmapStart = (uint64_t)superblockLoc + 1;
	ret->rootInode = 0;
	ret->badInode = 0;

	if (volumeLabel.length() > 63)
	{
		memcpy(ret->volumeLabel, volumeLabel.c_str(), 63);
		ret->volumeLabel[63] = 0;
	}
	else
	{
		memcpy(ret->volumeLabel, volumeLabel.c_str(), volumeLabel.length());
		ret->volumeLabel[volumeLabel.length()] = 0;
	}

	for (int i = 0; i < 16; i++)
	{
		ret->uuid[i] = (uint8_t)(rand() % 256);
	}

	this->bitmap = (uint8_t*) calloc(1, (size / 8));

	for (int i = 0; i < superblockLoc; i++)
	{
		this->markBitmap(i, 1);
	}

	this->markBitmap(superblockLoc, 1);
	this->markBitmap(superblockLoc + 1, 1);
	this->markBitmap(size - 1, 1);
	
	this->superblock = ret;

	Inode* rootInode = this->makeInode(iaSTD_DIR, superblockLoc);
	
	superblock->rootInode = rootInode->extentStarts[0];

	this->commitInode(rootInode);

	DirectoryEntry* de = this->createDirectoryEntry(rootInode, FT_DIRECTORY, ".");
	this->commitDirectoryEntry(rootInode, de);
	delete de;

	de = this->createDirectoryEntry(rootInode, FT_DIRECTORY, "..");

	this->commitDirectoryEntry(rootInode, de);
	delete de;

	delete rootInode;

}

void LeanFS::commitSuperblock()
{
	this->superblock->checksum = this->computeChecksum(this->superblock, 512);

	this->fsFile.seekp(this->superblock->primarySuper * 512);
	this->fsFile.write((char*)(this->superblock), 512);

	this->fsFile.seekp(this->superblock->backupSuper * 512);
	this->fsFile.write((char*)(this->superblock), 512);

	this->commitBitmap();
}

Inode* LeanFS::makeInode(uint32_t attribs, uint32_t parentNode)
{
	return this->makeInode(attribs, 0, parentNode);
}

Inode* LeanFS::makeInode(uint32_t attribs, uint32_t size, uint32_t parentNode)
{
	size += this->superblock->preallocCount;
	uint32_t sector = this->findNextLocation(size + 1, parentNode);

	assert(sector >= 0);

	Inode* ret = new Inode();

	ret->magic = 0x45444F4E;
	ret->extentCount = 1;
	ret->linkCount = 0;
	ret->attributes = attribs;
	ret->fileSize = 0;
	ret->sectorCount = (uint64_t)size + 1;
	ret->extentStarts[0] = sector;
	ret->extentSizes[0] = size + 1;

	this->superblock->freeSectorCount -= ret->sectorCount;

	return ret;
}

Inode* LeanFS::findInode(uint32_t inodeAddress)
{
	Inode* ret = new Inode();

	this->fsFile.seekg((uint64_t)inodeAddress * 512);
	this->fsFile.read((char*)ret, 176);

	uint32_t checksum = this->computeChecksum(ret, 176);

	if (ret->checksum != checksum)
	{
		std::cout << "Error reading inode " << inodeAddress;
		exit(1);
	}

	return ret;
}

void LeanFS::commitInode(Inode* inode)
{
	inode->checksum = this->computeChecksum(inode, 176);
	this->fsFile.seekp(inode->extentStarts[0] * 512);
	this->fsFile.write((char*)(inode), sizeof(Inode));

	for (int j = 0; j < inode->extentCount; j++)
	{
		uint32_t location = inode->extentStarts[j];

		for (int i = 0; i < inode->extentSizes[j]; i++)
		{
			this->markBitmap(location + i, 1);
		}
	}

	this->commitBitmap();
	this->commitSuperblock();
}

DirectoryEntry* LeanFS::createDirectoryEntry(Inode* linkedInode, uint8_t type, std::string name)
{
	DirectoryEntry* ret = new DirectoryEntry();

	uint8_t recLen = ((name.length() + 12) / 16) + (((name.length() % 16) != 0));

	ret->inode = linkedInode->extentStarts[0];
	ret->type = type;
	ret->recLen = recLen;
	ret->nameLen = name.length();
	ret->name = std::shared_ptr<char>((char*) calloc(1, name.length()));

	memcpy(ret->name.get(), name.c_str(), name.length());

	linkedInode->linkCount++;

	this->commitInode(linkedInode);

	return ret;
}

DirectoryEntry* LeanFS::findDirectoryEntry(Inode* inode, std::string name)
{
	for (int i = 0; i < inode->extentCount; i++)
	{
		for (int j = ((i == 0) ? 1 : 0); j <= inode->extentSizes[i]; j++)
		{
			int count = 0;
			uint32_t dirLocation = inode->extentStarts[i] + j;
			uint8_t buffer[512];
			this->fsFile.seekp((uint64_t)dirLocation * 512);
			this->fsFile.read((char*)buffer, 512);

			while (count < 512)
			{
				DirectoryEntry* header = (DirectoryEntry*)&buffer[count];

				if (header->inode != 0 && header->type != 0)
				{
					char* headerString = (char*)&buffer[count + 12];
					if (name.compare(headerString) == 0)
					{
						headerString = (char*) calloc(1, header->nameLen + 1);
						memcpy(headerString, &buffer[count + 12], header->nameLen + 1);
						header->name = std::shared_ptr<char>((char*)calloc(1, name.length()));

						memcpy(header->name.get(), headerString, header->nameLen + 1);

						free(headerString);

						DirectoryEntry* ret = new DirectoryEntry();

						memcpy(ret, header, sizeof(DirectoryEntry));

						return ret;
					}
				}

				count += (header->recLen * 16);
			}
		}
	}

	return 0;
}

void LeanFS::commitDirectoryEntry(Inode* parentInode, DirectoryEntry* directoryEntry)
{
	uint32_t count = 0;

	for (int i = 0; i < parentInode->extentCount; i++)
	{
		for (int j = ((i == 0) ? 1 : 0); j <= parentInode->extentSizes[i]; j++)
		{
			count = 0;
			uint32_t dirLocation = parentInode->extentStarts[i] + j;
			uint8_t buffer[512];
			this->fsFile.seekp((uint64_t) dirLocation * 512);
			this->fsFile.read((char*)buffer, 512);
			
			while (count < 512)
			{
				DirectoryEntry* header = (DirectoryEntry*)&buffer[count];
				if (header->inode == 0 && header->type == 0)
				{
					std::shared_ptr<char> data((char*) calloc(1, directoryEntry->recLen * 16));
					memcpy(data.get(), directoryEntry, 12);
					memcpy((data.get() + 12), directoryEntry->name.get(), directoryEntry->nameLen);
					
					this->fsFile.seekp((uint64_t) dirLocation * 512 + count);
					this->fsFile.write(data.get(), (uint64_t) directoryEntry->recLen * 16);

					parentInode->fileSize += (uint64_t) directoryEntry->recLen * 16;

					this->commitInode(parentInode);
					return;
				}
				else
				{
					count += (header->recLen * 16);
				}
			}
		}
	}
}

void LeanFS::markBitmap(uint32_t sector, uint8_t status)
{
	if (!status)
	{
		this->bitmap[sector / 8] = (((this->bitmap[sector / 8] & ((1 << (sector % 8)))) > 0) ? (this->bitmap[sector / 8] ^ (1 << (sector % 8))) : this->bitmap[sector / 8]);
	}
	else
	{
		this->bitmap[sector / 8] = this->bitmap[sector / 8] | (1 << (sector % 8));
	}
}

uint8_t LeanFS::checkBitmap(uint32_t sector)
{
	return ((this->bitmap[sector / 8] & (1 << (sector % 8))) > 0);
}

void LeanFS::commitBitmap()
{
	this->fsFile.seekp(this->superblock->bitmapStart * 512);
	this->fsFile.write((char*)(this->bitmap), this->superblock->sectorCount / 8);
}

uint32_t LeanFS::findNextLocation(uint32_t size, uint32_t currentLocation)
{
	uint32_t start = currentLocation;
	uint32_t maxCon = 0;

	for (int i = currentLocation; i < superblock->sectorCount; i++)
	{
		if (this->checkBitmap(i) == 0)
		{
			maxCon += 1;

			if (maxCon == size)
			{
				return start;
			}
		}
		else
		{
			maxCon = 0;
			start = (i + 1);
		}
	}
	return -1;
}
