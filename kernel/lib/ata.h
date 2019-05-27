#ifndef _atah
#define _atah

//ata_initdrive
//Initalizes the first PATA drive for use
void ata_initdrive();

//ata_readsector
//Param: LBA-the Logical Block Address of the location to be read
//Takes the location on the disk to be read and reads one sector(512 bytes)
//Returns: A 512 byte array of the data read from the location provided
unsigned char* ata_readsector(unsigned int LBA);

//ata_readsectors
//Param: LBA-the Logical Block Address of the location to be read, sectors-the number of sectors to read(Limit of 255 sectors)
//Takes the location on the disk to be read and reads the number of sectors specified
//Returns: A 512 byte times the number of sectors byte array of the data read from the location provided
//TODO: Implement
unsigned char* ata_readsectors(unsigned int LBA, unsigned char sectors);

//ata_waitfordrive
//Waits for the drive to be ready for the next command
void ata_waitfordrive();

//ata_waitrw
//Waits for the drive to be ready with data to read or write
void ata_waitrw();

//ata_writesector
//Params:  LBA-the Logical Block Address of the location to be written to, data-A 512 byte array with the data to be written (NOTE: Has to be 512 bytes, will write 512 bytes anyway)
//Writes data to a single sector at the location provieded
void ata_writesector(unsigned int LBA, unsigned char* data);

//ata_writebytes
//Params: LBA-the Logical Block Address of the location to be written to, offset-the number of bytes from the start of the sector to start writing to, data-the data to be written, dataSize-the size of the data to be written
//Writes an arbetrary amount of bytes to the location provided
//Corrects any bound errors with the offset, ie. if the offset is >= 512
void ata_writebytes(unsigned int LBA, unsigned short offset, unsigned char* data, unsigned int dataSize);

//ata_readbytes
//Params: LBA-the Logical Block Address of the location to be written to, offset-the number of bytes from the start of the sector to start writing to, size-the amount of the data to be read(in bytes)
//Reads an arbetrary amount of bytes from the location provided
//Corrects any bound errors with the offset, ie. if the offset is >= 512
//Returns: The array with the data read from the location provided
unsigned char* ata_readbytes(unsigned int LBA, unsigned int offset, unsigned int size);

//ata_updateinterrupt
//Sets the hasinterrupt bit for internal use
//void ata_updateinterrupt();

#endif