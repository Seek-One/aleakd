#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "../config.h"

#include "aleakd-data.h"

static int g_bIsInitializing = 0;
static void* (*real_malloc)(size_t) = NULL;
static void* (*real_calloc)(size_t, size_t) = NULL;
static void (*real_free)(void*) = NULL;
static void* (*real_realloc)(void*, size_t) = NULL;

// Dummy buffer to wrap initialization because dlsym can call malloc function
char tmpbuf[4096];
unsigned long tmppos = 0;
unsigned long tmpallocs = 0;

void* dummy_malloc(size_t size)
{
    if (tmppos + size >= sizeof(tmpbuf)) exit(1);
    void *retptr = tmpbuf + tmppos;
    tmppos += size;
    ++tmpallocs;
    return retptr;
}

void* dummy_calloc(size_t nmemb, size_t size)
{
    void *ptr = dummy_malloc(nmemb * size);
    unsigned int i = 0;
    for (; i < nmemb * size; ++i)
        *((char*)(ptr + i)) = '\0';
    return ptr;
}

void dummy_free(void *ptr)
{

}

void wrapper_init()
{
	if(g_bIsInitializing == 0)
	{
		g_bIsInitializing = 1;
		fprintf(stderr, "[aleakd] wrapper_init\n");

		aleakd_data_init_thread_list();

		// Free store data
		for (int i = 0; i < MAX_THREAD_COUNT; i++)
		{
			// Init thread
			struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(i);
			ThreadEntry_Reset(pThreadEntry);
#ifndef USE_AUTO_THREAD_START
			//printf("mymalloc started\n");
			pThreadEntry->iDetectionStarted = 1;
#endif

			pThreadEntry->alloc_list.max_count = TAB_SIZE;
			pThreadEntry->alloc_list.list = aleakd_data_get_alloc_list(i);
			AllocList_Clear(&pThreadEntry->alloc_list);
		}

		if(getenv("ALEAKD_MIN_ALLOC_NUM")){
			aleakd_data_set_display_min_alloc_num(atoi(getenv("ALEAKD_MIN_ALLOC_NUM")));
		}

		// Wrap functions
		real_malloc = dlsym(RTLD_NEXT, "malloc");
		if (NULL == real_malloc) {
			fprintf(stderr, "[aleakd] Error in `dlsym`: %s\n", dlerror());
		}

		real_calloc = dlsym(RTLD_NEXT, "calloc");
		if (NULL == real_calloc) {
			fprintf(stderr, "[aleakd] Error in `dlsym`: %s\n", dlerror());
		}

		real_free = dlsym(RTLD_NEXT, "free");
		if (NULL == real_free) {
			fprintf(stderr, "[aleakd] Error in `dlsym`: %s\n", dlerror());
		}

		real_realloc = dlsym(RTLD_NEXT, "realloc");
		if (NULL == real_realloc) {
			fprintf(stderr, "[aleakd] Error in `dlsym`: %s\n", dlerror());
		}

		fprintf(stderr, "[aleakd] wrapper_init done\n");
		//fprintf(stderr, "[aleakd] done");
#ifdef START_ON_CONSTRUCT
		g_bIsInitializing = 0;
#endif
	}
}

int displayEntry(struct ThreadEntry* pThread, struct AllocEntry* pAllocEntry)
{
	if(pAllocEntry->alloc_num < aleakd_data_get_display_min_alloc_num())
	{
		//if(pThread->name && pThread->iAllocCount > 10){
		//	return 0;
		//}
		return 0;
	}
	return 1;
}

void addEntry(void* ptr, size_t size, char* szAction)
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	struct ThreadEntry* pThreadEntry;

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdx(pThreadEntryList, thread);
	if(idx == -1) {
		idx = ThreadEntry_getIdxAdd(pThreadEntryList, thread, TAB_SIZE);
		if(idx != -1) {
			pThreadEntry = ThreadEntry_getByIdx(pThreadEntryList, idx);
#ifndef USE_AUTO_THREAD_START
			pThreadEntry->iDetectionStarted = 1;
#endif
		}
	}
	if(idx != -1) {
		struct AllocEntry allocEntry;
		allocEntry.ptr = ptr;
		allocEntry.size = size;
		allocEntry.thread = thread;
		allocEntry.alloc_num = aleakd_data_get_alloc_number();

		pThreadEntry = ThreadEntry_getByIdx(pThreadEntryList, idx);
		if (pThreadEntry->iDetectionStarted)
		{
			AllocList_Add(&pThreadEntry->alloc_list, &allocEntry);

			pThreadEntry->iCurrentSize += size;
			pThreadEntry->iAllocCount++;
			if (pThreadEntry->iCurrentSize > pThreadEntry->iMaxSize) {
				pThreadEntry->iMaxSize = pThreadEntry->iCurrentSize;
				if (aleakd_data_get_enable_print()) {
					//ThreadEntry_print(&g_listThread[idx], size);
				}
			}
			if (displayEntry(pThreadEntry, &allocEntry)) {
				fprintf(stderr,
						"[aleakd] thread %lu (%s): %s ptr=%p, size=%lu bytes, alloc_num=%lu => thread state: alloc_count=%d, memory=%lu bytes\n",
						pThreadEntry->thread, (pThreadEntry->name ? pThreadEntry->name : ""),
						szAction, ptr, size, allocEntry.alloc_num,
						pThreadEntry->iAllocCount, pThreadEntry->iCurrentSize);
			}
			if (aleakd_data_get_alloc_number() == aleakd_data_get_break_alloc_num()) {
				fprintf(stderr, "[aleakd] break\n");
			}
		}
	}
}

void removeEntry(void* ptr, char* szAction)
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	struct ThreadEntry* pThreadEntry;

	struct AllocEntry allocEntry;

	int iFound = 0;
	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdx(pThreadEntryList, thread);
	if(idx != -1) {
		pThreadEntry = ThreadEntry_getByIdx(pThreadEntryList, idx);
		if (pThreadEntry->iDetectionStarted) {
			iFound = AllocList_Remove(&pThreadEntry->alloc_list, ptr, &allocEntry);
			if(iFound){
				pThreadEntry->iCurrentSize-=allocEntry.size;
				pThreadEntry->iAllocCount--;
				if(displayEntry(pThreadEntry, &allocEntry)){
					fprintf(stderr, "[aleakd] thread %lu (%s): %s %p (%lu bytes) => alloc_count=%d, memory=%lu bytes\n",
							pThreadEntry->thread, (pThreadEntry->name ? pThreadEntry->name : ""),
							szAction, ptr, allocEntry.size,
							pThreadEntry->iAllocCount, pThreadEntry->iCurrentSize);
				}
			}
		}
	}
	// Look in other thread if not found
	if(iFound == 0){
		for(int i=0; i<MAX_THREAD_COUNT; i++){
			pThreadEntry = ThreadEntry_getByIdx(pThreadEntryList, i);
			if ((i != idx) && pThreadEntry->iDetectionStarted) {
				iFound = AllocList_Remove(&pThreadEntry->alloc_list, ptr, &allocEntry);
				if(iFound){
					pThreadEntry->iCurrentSize-=allocEntry.size;
					pThreadEntry->iAllocCount--;
					if(displayEntry(pThreadEntry, &allocEntry)){
						fprintf(stderr, "[aleakd] thread::fallback %lu (%s): %s %p (%lu bytes) => alloc_count=%d, memory=%lu bytes\n",
							pThreadEntry->thread, (pThreadEntry->name ? pThreadEntry->name : ""),
		   					 szAction, ptr, allocEntry.size,
							pThreadEntry->iAllocCount, pThreadEntry->iCurrentSize);
					}
				}
			}
		}
	}
}

void *malloc(size_t size)
{
	void *p = NULL;

	aleakd_data_incr_alloc_number();

	if(!real_malloc){
		wrapper_init();
	}

	if(real_malloc){
		p = real_malloc(size);
	}else{
		p = dummy_malloc(size);
	}

	if(p){
		addEntry(p, size, "malloc");
	}

	return p;
}

void *calloc(size_t num, size_t size)
{
	aleakd_data_incr_alloc_number();

	if(!real_calloc){
		wrapper_init();
	}

	void *p = NULL;
	if(real_calloc){
		p = real_calloc(num, size);
	}else{
		p = dummy_calloc(num, size);
	}

	if(p){
		addEntry(p, size*num, "calloc");
	}

	return p;
}


void *realloc(void* ptr, size_t size)
{
	if(!real_realloc){
		wrapper_init();
	}

	if(ptr){	
		removeEntry(ptr, "realloc::free");
	}

	void *p = NULL;
	p = real_realloc(ptr, size);
	
	if(p){
		addEntry(p, size, "realloc");
	}

	return p;
}

void free(void *ptr)
{
	if(!real_free){
		wrapper_init();
	}

	if(ptr != NULL) {
		removeEntry(ptr, "free");
	}

	if(real_free){
		real_free(ptr);
	}else{
		dummy_free(ptr);
	}
}


