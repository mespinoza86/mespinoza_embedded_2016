#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>


int free_count=0; 
int malloc_count=0;


static void* (*real_malloc)(size_t)=NULL;

static void mtrace_init(void)
{
	real_malloc = dlsym(RTLD_NEXT, "malloc");
	if (NULL == real_malloc) {
		fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
	}
}


void mem_leaks_analysis (){
	int mem_free; 
	int mem_leaks;

	mem_free = free_count;
	mem_leaks = malloc_count - free_count;

	fprintf(stderr,"Analysis finished!\n");	
	fprintf(stderr,"Memory allocations: %i\n", malloc_count);
	fprintf(stderr,"Memory free: %i\n", free_count);
	fprintf(stderr,"Total memory leaks found %i\n", mem_leaks);

}

void *malloc(size_t size)
{
	if(real_malloc==NULL) {
		mtrace_init();
	}

	void *p = NULL;
	fprintf(stderr, "malloc(%d) = ", size);
	p = real_malloc(size);
	fprintf(stderr, "%p\n", p);
	malloc_count++;
	return p;
}


void free(void *p)
{
	void (*libc_free)(void*) = dlsym(RTLD_NEXT, "free");
	fprintf(stderr,"free %p\n", p);
	libc_free(p);
	free_count++;
	mem_leaks_analysis();
}


