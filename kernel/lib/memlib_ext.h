//This is the extensions to the memlib
//In here are the extra functions needed to implement a malloc and free
#ifndef _memlib_ext_h
#define _memlib_ext_h

struct kpage
{
	struct kpage* nextPage;
	void* location;
};

struct kbucket
{
	struct kbucket* nextBucket;
	struct kpage* headPage;
	unsigned int size;
};



void free_pages(struct kbucket* bucket);
void remove_free_page(struct kpage* page);
struct kpage* get_next_contiguous_pages(int numPages);
struct kpage* allocNewPage(void* location);
struct kbucket* allocNewBucket();

#endif