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

//ata_waitfordrive
//Waits for the drive to be ready for the next command
void ata_waitfordrive();

//ata_writesector
//Params:  LBA-the Logical Block Address of the location to be read, data-A 512 byte array with the data to be written (NOTE: Has to be 512 bytes, will write 512 bytes anyway)
//Writes data to a single sector at the location provieded
void ata_writesector(unsigned int LBA, unsigned char* data);

#endif