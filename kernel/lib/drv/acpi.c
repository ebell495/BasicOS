#include "acpi.h"
#include "hwio.h"
#include "../util/utils.h"
#include "../util/memlib.h"

struct RSDPDescriptor findRSDP()
{
	//Check the lower 1KB for the header
	char* ptr = 0;
	while(ptr < 1024)
	{
		if(memcmp(ptr, "RSD PTR ", 8) == 0)
		{
			return *((struct RSDPDescriptor*) ptr);
		}

		ptr += 16;
	}

	//Search the main bios area under 1mb
	ptr = 0xE0000;
	while(ptr < 0xFFFFF)
	{
		if(memcmp(ptr, "RSD PTR ", 8) == 0)
		{
			return *((struct RSDPDescriptor*) ptr);
		}

		ptr += 16;
	}

	while(1);
}

struct FADT* parseFADT(struct RSDPDescriptor* rsdp)
{
	struct FADT* ret = kmalloc(sizeof(struct FADT));

	struct ACPISDTHeader* h = ((struct ACPISDTHeader*) rsdp->rsdtAddress);

	int numHeaders = (h->length - sizeof(struct ACPISDTHeader)) / 4;

	for(int i = 0; i < numHeaders; i++)
	{
		if(memcmp((char*)*((unsigned int*)((char*)h + (4*i) + sizeof(struct ACPISDTHeader))), "FACP", 4) == 0)
		{
			memcpy(ret, (char*)*((unsigned int*)((char*)h + (4*i) + sizeof(struct ACPISDTHeader))), sizeof(struct FADT));
			break;
		}
	}

	return ret;
}

void printFADT(struct FADT* fadt)
{
	p_serial_printf("FADT Header information\n");

}

void dumpDSDT(struct FADT* fadt)
{
	unsigned char* dsdtTable = fadt->Dsdt;

	struct ACPISDTHeader* dsdtHeader = (struct ACPISDTHeader*) dsdtTable;

	p_serial_printf("DSDT Header information\n");
	p_serial_printf("DSDT Signature: %s\n", dsdtHeader->signature);
	p_serial_printf("DSDT Length: %i\n", dsdtHeader->length);
	p_serial_printf("DSDT OEMID: %s\n", dsdtHeader->OEMID);

	for(int i = 0; i < dsdtHeader->length; i++)
	{
		p_serial_write(dsdtTable[i]);
	}
}