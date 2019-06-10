#include "ata.h"
#include "hwio.h"
#include "../util/memlib.h"
#include "display.h"

//Drive Ports
#define ATA_PRI_DATA_REG 0x1F0
#define ATA_PRI_ERROR_REG 0x1F1
#define ATA_PRI_SEC_COUNT_REG 0x1F2
#define ATA_PRI_LBA_LOW_REG 0x1F3
#define ATA_PRI_LBA_MID_REG 0x1F4
#define ATA_PRI_LBA_HIGH_REG 0x1F5
#define ATA_PRI_DRIVE_REG 0x1F6
#define ATA_PRI_STATUS_COM_REG 0x1F7

//Drive Commands
#define ATA_COM_DEVICE_RESET 0x08
#define ATA_COM_READ_W_RETRY 0x20
#define ATA_COM_WRITE_W_RETRY 0x30
#define ATA_COM_SMART 0xB0
#define ATA_COM_FLUSH 0xE7
#define ATA_COM_IDENTITY 0xEC

//ATA Status Bits
#define ATA_STATUS_ERR_BIT  0b00000001    //Set if there was an error
#define ATA_STATUS_IDX_BIT  0b00000010
#define ATA_STATUS_CORR_BIT 0b00000100
#define ATA_STATUS_DRQ_BIT  0b00001000    //Set when the drive is ready to send/receive data
#define ATA_STATUS_SRV_BIT  0b00010000    
#define ATA_STATUS_DF_BIT   0b00100000    //Set if there was a drive fault
#define ATA_STATUS_RDY_BIT  0b01000000	  //Clear if the drive has spun down
#define ATA_STATUS_BSY_BIT  0b10000000    //Set if the drive is preparing to send/receive data

unsigned int numSectors = 0;

void ata_initdrive()
{
	pbyteout(ATA_PRI_DRIVE_REG, 0xA0);
	
	/*

	unsigned char cl = pbytein(ATA_PRI_LBA_MID_REG);
	unsigned char ch = pbytein(ATA_PRI_LBA_HIGH_REG);

	if (cl==0x14 && ch==0xEB) disp_printstring("PATAPI Drive detected\n");
	else if (cl==0x69 && ch==0x96) disp_printstring("SATAPI Drive detected\n");
	else if (cl==0 && ch == 0) disp_printstring("PATA Drive detected\n");
	else if (cl==0x3c && ch==0xc3) disp_printstring("SATA Drive detectedd\n");
	else disp_printstring("Error, Drive Unknown\n");
	
	*/

	pbyteout(ATA_PRI_DRIVE_REG, 0xA0);
	
	pbyteout(ATA_PRI_SEC_COUNT_REG, 0);
	pbyteout(ATA_PRI_LBA_LOW_REG, 0);
	pbyteout(ATA_PRI_LBA_MID_REG, 0);
	pbyteout(ATA_PRI_LBA_HIGH_REG, 0);
	
	pbyteout(ATA_PRI_STATUS_COM_REG, ATA_COM_IDENTITY);
	
	ata_waitfordrive();
	
	unsigned short* driveInfo = kmalloc(512);
	
	for(int j = 0; j < 256; j++)
	{
		driveInfo[j] = pwordin(ATA_PRI_DATA_REG);
		if(j == 61)
		{
			/*
			disp_printstring("Number of LBA28 Sectors: ");
			disp_phex16(driveInfo[j]);
			disp_phex16(driveInfo[j-1]);
			disp_printc('\n');
			*/

			numSectors = ((((unsigned int)driveInfo[j] << 16)) + driveInfo[j-1]);
		}
	}

	kfree(driveInfo);
}

unsigned char* ata_readsector(unsigned int LBA)
{
	unsigned char* ret = kmalloc(512);	
	
	pbyteout(ATA_PRI_DRIVE_REG, 0xE0);
	pbyteout(ATA_PRI_ERROR_REG, 0);
		
	pbyteout(ATA_PRI_SEC_COUNT_REG, 1);
	
	pbyteout(ATA_PRI_LBA_LOW_REG, LBA & 0xFF);
	pbyteout(ATA_PRI_LBA_MID_REG, (LBA >> 8) & 0xFF);
	pbyteout(ATA_PRI_LBA_HIGH_REG, (LBA >> 16) & 0xFF);
	
	pbyteout(ATA_PRI_STATUS_COM_REG, ATA_COM_READ_W_RETRY);
	
	ata_waitfordrive();
	ata_waitrw();
	
	
	for(int j = 0; j < 256; j++)
	{
		unsigned short in = pwordin(ATA_PRI_DATA_REG);
		ret[j*2] = in & 0xFF;
		ret[j*2 + 1] = (in >> 8) & 0xFF;
	}
	
	return ret;
}

void ata_writesector(unsigned int LBA, unsigned char* data)
{
	pbyteout(ATA_PRI_DRIVE_REG, 0xE0);
	pbyteout(ATA_PRI_ERROR_REG, 0);
		
	pbyteout(ATA_PRI_SEC_COUNT_REG, 1);
	
	pbyteout(ATA_PRI_LBA_LOW_REG, LBA & 0xFF);
	pbyteout(ATA_PRI_LBA_MID_REG, (LBA >> 8) & 0xFF);
	pbyteout(ATA_PRI_LBA_HIGH_REG, (LBA >> 16) & 0xFF);
	
	pbyteout(ATA_PRI_STATUS_COM_REG, ATA_COM_WRITE_W_RETRY);
	
	ata_waitfordrive();
	ata_waitrw();
	
	for(int j = 0; j < 256; j++)
	{
		unsigned short out = data[j*2] + (data[j*2 + 1] << 8);
		pwordout(ATA_PRI_DATA_REG, out);
	}
	
	pbyteout(ATA_PRI_STATUS_COM_REG, ATA_COM_FLUSH);
	ata_waitfordrive();
}

void ata_waitfordrive()
{
	//Delay for 400ns
	pbytein(0x3F6);
	pbytein(0x3F6);
	pbytein(0x3F6);
	pbytein(0x3F6);
	
	unsigned char status = pbytein(ATA_PRI_STATUS_COM_REG);
	
	if((status & ATA_STATUS_ERR_BIT) == ATA_STATUS_ERR_BIT || (status & ATA_STATUS_DF_BIT) == ATA_STATUS_DF_BIT)
	{
		disp_printstring("Disk Error, Halting: Error Reg: ");
		disp_phex32(pbytein(ATA_PRI_ERROR_REG));
	}
	
	while((status & ATA_STATUS_BSY_BIT) == ATA_STATUS_BSY_BIT || (status & ATA_STATUS_RDY_BIT) != ATA_STATUS_RDY_BIT)
	{
		status = pbytein(ATA_PRI_STATUS_COM_REG);
	}
}

void ata_waitrw()
{
	unsigned char status = pbytein(ATA_PRI_STATUS_COM_REG);
	while((status & ATA_STATUS_DRQ_BIT) != ATA_STATUS_DRQ_BIT)
	{
		status = pbytein(ATA_PRI_STATUS_COM_REG);
	}
}

void ata_writebytes(unsigned int LBA, unsigned short offset, unsigned char* data, unsigned int dataSize)
{
	if(offset < 0)
		return;

	while(offset >= 512)
	{
		LBA++;
		offset -= 512;
	}

	if(dataSize + offset <= 512)
	{
		unsigned char* readData = ata_readsector(LBA);
		memcpy((void*)(readData + offset), data, dataSize);
		ata_writesector(LBA, readData);
		kfree(readData);
	}
	else
	{
		unsigned char* readData = ata_readsector(LBA);
		memcpy((void*)(readData + offset), data, 512-offset);
		dataSize -= (512-offset);
		ata_writesector(LBA, readData);
		kfree(readData);
		LBA++;
		int count = 0;

		while(dataSize > 512)
		{
			ata_writesector(LBA, (void*)(data + offset + (count * 512)));
			LBA++;
			dataSize -= 512;
			count++;
		}

		readData = ata_readsector(LBA);
		memcpy((void*)(readData), (void*)(data + offset + (count * 512)), dataSize);
		ata_writesector(LBA, readData);
		kfree(readData);
	}
}

unsigned char* ata_readbytes(unsigned int LBA, unsigned int offset, unsigned int size)
{
	unsigned char* ret = kmalloc(size);

	if(offset < 0)
		return 0;

	while(offset >= 512)
	{
		LBA++;
		offset -= 512;
	}

	if(size + offset <= 512)
	{
		unsigned char* readData = ata_readsector(LBA);
		memcpy((void*)ret, (void*)(readData + offset), size);
		kfree(readData);
		return ret;
	}
	else
	{
		unsigned int amtRead = 0;
		unsigned char* readData = ata_readsector(LBA);
		memcpy((void*)ret, (void*)(readData + offset), 512-offset);
		kfree(readData);
		size -= (512-offset);
		amtRead += (512-offset);
		LBA+=1;

		while(size > 512)
		{
			readData = ata_readsector(LBA);
			memcpy((void*)(ret + amtRead), readData, 512);
			kfree(readData);
			LBA+=1;
			size -= 512;
			amtRead += 512;
			
		}

		readData = ata_readsector(LBA);
		memcpy((void*)(ret + amtRead), readData, size);
		kfree(readData);
	}
	return ret;
}

unsigned int ata_getNumSectors()
{
	return numSectors;
}

