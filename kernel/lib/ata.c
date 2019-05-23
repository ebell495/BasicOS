#include "ata.h"
#include "hwio.h"
#include "memlib.h"
#include "display.h"

void ata_initdrive()
{
	pbyteout(0x1F6, 0xA0);
	
	unsigned char cl = pbytein(0x1F0 + 4);
	unsigned char ch = pbytein(0x1F0 + 5);
	
	if (cl==0x14 && ch==0xEB) disp_printstring("PATAPI Drive detected\n");
	else if (cl==0x69 && ch==0x96) disp_printstring("SATAPI Drive detected\n");
	else if (cl==0 && ch == 0) disp_printstring("PATA Drive detected\n");
	else if (cl==0x3c && ch==0xc3) disp_printstring("SATA Drive detectedd\n");
	else disp_printstring("Error, Drive Unknown\n");
	
	pbyteout(0x1F6, 0xA0);
	
	pbyteout(0x1F2, 0);
	pbyteout(0x1F3, 0);
	pbyteout(0x1F4, 0);
	pbyteout(0x1F5, 0);
	
	pbyteout(0x1F7, 0xEC);
	
	ata_waitfordrive();
	
	unsigned short* driveInfo = kmalloc(512);
	
	for(int j = 0; j < 256; j++)
	{
		driveInfo[j] = pwordin(0x1F0);
		if(j == 61)
		{
			disp_printstring("Number of LBA28 Sectors: ");
			disp_phex16(driveInfo[j]);
			disp_phex16(driveInfo[j-1]);
			disp_printc('\n');
		}
	}
}

unsigned char* ata_readsector(unsigned int LBA)
{
	unsigned char* ret = kmalloc(512);
	
	p_serial_writestring("ReadSector: Start. LBA: ");
	p_serial_writenum(LBA);
	
	
	pbyteout(0x1F6, 0xE0);
	pbyteout(0x1F1, 0);
		
	pbyteout(0x1F2, 1);
	
	pbyteout(0x1F3, LBA & 0xFF);
	pbyteout(0x1F4, (LBA >> 8) & 0xFF);
	pbyteout(0x1F5, (LBA >> 16) & 0xFF);
	
	pbyteout(0x1F7, 0x20);
	
	p_serial_writestring("\nReadSector: Sent read command for 1 sector\n");
	p_serial_writestring("\nReadSector: Going to WAIT for Drive\n");
	ata_waitfordrive();
	p_serial_writestring("\nReadSector: Going to WAIT R/W for Drive\n");
	ata_waitrw();
	
	p_serial_writestring("\nReadSector: Reading Data\n\n");
	
	for(int j = 0; j < 256; j++)
	{
		unsigned short in = pwordin(0x1F0);
		ret[j*2] = in & 0xFF;
		ret[j*2 + 1] = (in >> 8) & 0xFF;
	}
	
	p_serial_writestring("\n\nReadSector: Read Data; Exiting\n");
	
	return ret;
}

void ata_writesector(unsigned int LBA, unsigned char* data)
{
	p_serial_writestring("WriteSector: Start. LBA: ");
	p_serial_writenum(LBA);
	
	pbyteout(0x1F6, 0xE0);
	pbyteout(0x1F1, 0);
		
	pbyteout(0x1F2, 1);
	
	pbyteout(0x1F3, LBA & 0xFF);
	pbyteout(0x1F4, (LBA >> 8) & 0xFF);
	pbyteout(0x1F5, (LBA >> 16) & 0xFF);
	
	pbyteout(0x1F7, 0x30);
	
	p_serial_writestring("\nWriteSector: Sent read command for 1 sector\n");
	p_serial_writestring("\nWriteSector: Going to WAIT for Drive\n");
	ata_waitfordrive();
	p_serial_writestring("\nWriteSector: Going to WAIT R/W for Drive\n");
	ata_waitrw();
	
	p_serial_writestring("\nWriteSector: Reading Data\n\n");
	
	for(int j = 0; j < 256; j++)
	{
		unsigned short out = data[j*2] + (data[j*2 + 1] << 8);
		pwordout(0x1F0, out);
	}
	
	pbyteout(0x1F7, 0xE7);
	p_serial_writestring("\nWriteSector: Going to WAIT for Drive to flush\n");
	ata_waitfordrive();
	
	p_serial_writestring("\nWriteSector: Read Data; Exiting\n");
}

void ata_waitfordrive()
{
	p_serial_writestring("\nATA_WAIT\n");
	//Delay for 400ns
	pbytein(0x3F6);
	pbytein(0x3F6);
	pbytein(0x3F6);
	pbytein(0x3F6);
	
	unsigned char status = pbytein(0x1F7);
	
	p_serial_write(status);
	
	if((status & 0b00000001) == 0b00000001 || (status & 0b00100000) == 0b00100000)
	{
		disp_printstring("Disk Error, Halting: Error Reg: ");
		disp_phex32(pbytein(0x1F1));
	}
	
	while((status & 0b10000000) == 0b10000000 || (status & 0b01000000) != 0b01000000)
	{
		status = pbytein(0x1F7);
		p_serial_write(status);
	}
}

void ata_waitrw()
{
	p_serial_writestring("\nATA_WAIT RW\n");
	unsigned char status = pbytein(0x1F7);
	p_serial_write(status);
	while((status & 0b00001000) != 0b00001000)
	{
		status = pbytein(0x1F7);
		p_serial_write(status);
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
		memcpy((void*)(readData + offset), data, dataSize-offset);
		ata_writesector(LBA, readData);
	}
	else
	{
		unsigned char* readData = ata_readsector(LBA);
		memcpy((void*)(readData + offset), data, 512-offset);
		dataSize -= (512-offset);
		ata_writesector(LBA, readData);
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
	}
}

unsigned char* ata_readbytes(unsigned int LBA, unsigned int offset, unsigned int size)
{
	unsigned char* ret = kmalloc(size);

	p_serial_writestring("\nRead Bytes: Size: ");
	p_serial_writenum(size);
	p_serial_writestring(" Offset: ");
	p_serial_writenum(offset);
	p_serial_writestring(" LBA: ");
	p_serial_writenum(LBA);
	p_serial_write('\n');

	if(offset < 0)
		return 0;

	while(offset >= 512)
	{
		LBA++;
		offset -= 512;
	}

	p_serial_writestring("New Offset: ");
	p_serial_writenum(offset);
	p_serial_writestring(" LBA: ");
	p_serial_writenum(LBA);
	p_serial_write('\n');

	if(size + offset <= 512)
	{
		unsigned char* readData = ata_readsector(LBA);
		memcpy((void*)ret, (void*)(readData + offset), size);

		return ret;
	}
	else
	{
		unsigned int amtRead = 0;
		unsigned char* readData = ata_readsector(LBA);
		memcpy((void*)ret, (void*)(readData + offset), 512-offset);
		size -= (512-offset);
		amtRead += (512-offset);
		LBA+=1;

		p_serial_writestring("New Size: ");
		p_serial_writenum(size);
		p_serial_writestring(" LBA: ");
		p_serial_writenum(LBA);
		p_serial_writestring(" Amount Read: ");
		p_serial_writenum(amtRead);
		p_serial_write('\n');

		while(size > 512)
		{
			readData = ata_readsector(LBA);
			memcpy((void*)(ret + amtRead), readData, 512);
			LBA+=1;
			size -= 512;
			amtRead += 512;
			p_serial_writestring("New Size: ");
			p_serial_writenum(size);
			p_serial_writestring(" LBA: ");
			p_serial_writenum(LBA);
			p_serial_writestring(" Amount Read: ");
			p_serial_writenum(amtRead);
			p_serial_write('\n');
		}

		readData = ata_readsector(LBA);
		memcpy((void*)(ret + amtRead), readData, size);
	}
	return ret;
}

