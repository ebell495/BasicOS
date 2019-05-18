#include "memlib.h"
#include "memlib_ext.h"
#include "display.h"
#define E820_LOC 0x10000
#define E820_COUNT_LOC 0xFFFD
#define K_PAGE_SIZE 64


unsigned int lowMemSize = 0x0;
unsigned int highMemLocation = 0x0;
unsigned int highMemSize = 0x0;

unsigned int cHeapSize = 0;

struct kpage* freePages = 0x0;
struct kbucket* usedBuckets = 0x0;

unsigned int bucketStartLocation = 0x0;

unsigned int nextPageLocation = 0x0;
unsigned int nextBucketLocation = 0x0;

void* memcpy(void* restrict dstptr, const void* restrict srcptr, int size) 
{
	unsigned char* dst = ((unsigned char*) dstptr);
	const unsigned char* src = ((const unsigned char*) srcptr);
	for (int i = 0; i < size; i++)
	{
		*dst = *src;
		dst++;
		src++;
	}
	return dstptr;
}

//Kernel memory allocation

void* kmalloc(unsigned int size)
{
	/*
	if(size <= 0)
		return (void*)-1;
	
	int numPages = (size / K_PAGE_SIZE) + 1;
	struct kpage* hPage = get_next_contiguous_pages(numPages);
	
	struct kbucket* b = allocNewBucket(numPages * K_PAGE_SIZE, hPage, usedBuckets);
	usedBuckets = b;
	
	return hPage->location;
	*/
	
	int numPages = (size / K_PAGE_SIZE) + 1;
	void* ret = (void*)bucketStartLocation;
	bucketStartLocation += numPages;
	
	return ret;
	
}

void kfree(void* pointer)
{
	return;
	/*
	//Make sure its a valid pointer
	if(((unsigned int)pointer) % K_PAGE_SIZE != 0)
	{
		disp_printstring("No such pointer0\n");
		return;
	}
	
	//Make sure there have been pointers allocated
	if(usedBuckets == 0x0)
	{
		disp_printstring("No such pointer1\n");
		return;
	}
	
	struct kbucket* tmpBucket = usedBuckets;
	if(tmpBucket->headPage->location == pointer)
	{
		free_pages(tmpBucket);
		usedBuckets = tmpBucket->nextBucket;
		//TODO: Free the buckets back somehow
		return;
	}
	
	//Finds the bucket that was allocated for the pointer
	while(tmpBucket->nextBucket != 0x0)
	{
		if(tmpBucket->nextBucket->headPage->location == pointer)
		{
			//Return the pages it used back into the pages availiable
			free_pages(tmpBucket->nextBucket);
			tmpBucket->nextBucket = tmpBucket->nextBucket->nextBucket;
			return;
		}
		
		tmpBucket = tmpBucket->nextBucket;
	}
	
	disp_printstring("No such pointer2\n");
	return;
	*/
}

//Reads the e820 map that was loaded during the boot sequence
//It then take the important information out of it and stores the sizes above

void mem_read_e820()
{
	int count = *((int*)(E820_COUNT_LOC));
	
	for(int i = 0; i < count; i++)
	{
		unsigned int baseAddress = 0x0;
		for(int j = 0; j < 4; j++)
		{
			baseAddress += (((unsigned char) *((int*)(E820_LOC + (24*i) + j))) << (j*8));
		}
		
		unsigned int size = 0x0;
		for(int j = 0; j < 4; j++)
		{
			size += ((unsigned char) *((int*)(E820_LOC + (24*i) + (8) + j))) << (j*8);
		}
		
		unsigned char type = (unsigned char) *((int*)(E820_LOC + (24*i) + (16)));
		
		/*
		disp_printc('\n');
		disp_phex8(i);
		disp_printc(' ');
		disp_phex32(baseAddress);
		disp_printc(' ');
		disp_phex32(size);
		disp_printc(' ');
		disp_phex8(type);
		*/
		
		if(type == 1)
		{
			//See if we have found the upper memory location
			if(baseAddress > 0x9FC00)
			{
				highMemLocation = baseAddress;
				highMemSize = size;
				
				bucketStartLocation = baseAddress + size - 16 - (16 - ((baseAddress + size) % 16));
				nextBucketLocation = bucketStartLocation;
				nextPageLocation = bucketStartLocation - (sizeof(struct kbucket) * ((size / 4096)));
				/*
				disp_phex32(bucketStartLocation);
				disp_printc('\n');
				disp_phex32(nextPageLocation);
				disp_printc('\n');
				disp_phex32(baseAddress);
				disp_printc('\n');
				disp_phex32(size);
				disp_printc('\n');
				*/
			}
			else
			{
				lowMemSize = size;
			}
		}
	}
}

void free_pages(struct kbucket* bucket)
{
	struct kpage* workingPage = bucket->headPage;
	
	//Check if there are any free pages already
	if(freePages == 0x0)
	{
		freePages = workingPage;
		return;
	}
	
	struct kpage* tmp = freePages;
	
	if(workingPage->location < tmp->location)
	{
		struct kpage* newEnd = tmp->nextPage;
		freePages = workingPage;
		
		struct kpage* t2 = workingPage;
		while(t2->nextPage !=  0x0)
		{
			t2 = t2->nextPage;
		}
		t2->nextPage = newEnd;
		return;
	}
	
	while(tmp->nextPage != 0x0)
	{
		//We want to return the pages in accending order
		if(workingPage->location < tmp->nextPage->location)
		{
			struct kpage* newEnd = tmp->nextPage;
			tmp->nextPage = workingPage;
			
			struct kpage* t2 = workingPage;
			while(t2->nextPage != 0x0)
			{
				t2 = t2->nextPage;
			}
			t2->nextPage = newEnd;
			return;
		}
		tmp = tmp->nextPage;
	}
	
	tmp->nextPage = workingPage;
}

//Removes the page from the freePage pile
void remove_free_page(struct kpage* page)
{
	struct kpage* tmp = freePages;
		
	if(tmp == page)
	{
		freePages = tmp->nextPage;
		page->nextPage = 0x0;
		return;
	}
	
	while(tmp->nextPage != 0x0)
	{
		if(tmp->nextPage == page)
		{
			tmp->nextPage = tmp->nextPage->nextPage;
			page->nextPage = 0x0;
			return;
		}
		tmp = tmp->nextPage;
	}
}

//This is the main kmalloc function
//Finds or allocates the pages of memory that will be used
struct kpage* get_next_contiguous_pages(int numPages)
{
	if(freePages == 0x0)
	{
		if(usedBuckets == 0x0)
		{
			//The best case, there are no free pages and there are no buckets in uses
			
			struct kpage* returnPage = allocNewPage((void*)highMemLocation);
			struct kpage* tmp = returnPage;
			
			for(int i = 1; i < numPages; i++)
			{
				tmp->nextPage = allocNewPage((void*)(highMemLocation + (i*K_PAGE_SIZE)));
				tmp = tmp->nextPage;
			}
			return returnPage;
		}
		
		struct kbucket* tmpBucket = usedBuckets;
		
		struct kpage* returnPage = allocNewPage(tmpBucket->headPage->location + tmpBucket->size);
		struct kpage* tmp = returnPage;
		for(int i = 1; i < numPages; i++)
		{
			tmp->nextPage = allocNewPage(returnPage->location + (i*K_PAGE_SIZE));
			tmp = tmp->nextPage;
		}
		return returnPage;
	}
		
	struct kpage* cPage = freePages;
	struct kpage* tmpPage = freePages;
	
	
	int countOfPages = 0;
	
	
	//This searches for a contiguous page collect of the size we need
	while(tmpPage != 0x0 && countOfPages != numPages)
	{
		if(countOfPages == 0)
			cPage = tmpPage;
		
		if(cPage->location == (tmpPage->location - (K_PAGE_SIZE * countOfPages)))
		{
			countOfPages++;
		}
		else
		{
			countOfPages = 0;
		}
		
		if(countOfPages == numPages)
		{
			break;
		}
		
		tmpPage = tmpPage->nextPage;
		
	}
	
	//Found the pages we want to use
	if(countOfPages == numPages)
	{
		struct kpage* tmp = cPage;
		struct kpage* nPage = tmp->nextPage;
		
		while(numPages > 0)
		{
			tmp->nextPage = nPage;
			remove_free_page(tmp);
			tmp = nPage;
			numPages--;
		}
		
		return cPage;
	}
	
	
	struct kbucket* lastBucket = usedBuckets;
	
	struct kbucket* tmpBucket = lastBucket;
	
	while(tmpBucket != 0x0)
	{
		if(lastBucket->headPage->location < tmpBucket->headPage->location)
			lastBucket = tmpBucket;
		tmpBucket = tmpBucket->nextBucket;
	}
	
	struct kpage* returnPage = allocNewPage(lastBucket->headPage->location + lastBucket->size);
	struct kpage* tmp = returnPage;
	for(int i = 1; i < numPages; i++)
	{
		tmp->nextPage = allocNewPage(returnPage->location + (i*K_PAGE_SIZE));
		tmp = tmp->nextPage;
	}
	return returnPage;
}

struct kpage* allocNewPage(void* location)
{
	struct kpage* ret = (struct kpage*)(nextPageLocation);
	nextPageLocation -= sizeof(struct kpage);
	ret->location = location;
	return ret;
}

struct kbucket* allocNewBucket(unsigned int size, struct kpage* headPage, struct kbucket* nextBucket)
{
	struct kbucket* ret = (struct kbucket*)(nextBucketLocation);
	nextBucketLocation -= sizeof(struct kbucket);
	
	ret->size = size;
	ret->headPage = headPage;
	ret->nextBucket = nextBucket;
	
	return ret;
}