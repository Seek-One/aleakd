
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "aleakd-data.h"

static void* (*real_malloc)(size_t)=NULL;
static void (*real_free)(void*)=NULL;
static void* (*real_realloc)(void*, size_t)=NULL;

static void mtrace_init(void)
{
	real_malloc = dlsym(RTLD_NEXT, "malloc");
	if (NULL == real_malloc) {
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

	for(int i=0; i<MAX_THREAD_COUNT; i++)
	{
		ThreadEntry_Reset(&g_listThread[i]);
#ifndef USE_THREAD_START
		//printf("mymalloc started\n");
		g_listThread[i].iDetectionStarted = 1;
#endif
		g_listAllocEntryList[i].max_count = TAB_SIZE;
		g_listAllocEntryList[i].list = g_listAllocEntry[i];
		AllocList_Reset(&g_listAllocEntryList[i]);
	}
}

int displayEntry(struct ThreadEntry* pThread, size_t size)
{
	if(pThread->name && pThread->name[0] == g_scanThread){
		return 1;
	}
	return 0;
}

void addEntry(void* ptr, size_t size, char* szAction)
{
	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdx(g_listThread, MAX_THREAD_COUNT, thread);
	if(idx == -1) {
		idx = ThreadEntry_getIdxAdd(g_listThread, MAX_THREAD_COUNT, thread, TAB_SIZE);
		if(idx != -1) {
#ifndef USE_THREAD_START
			g_listThread[idx].iDetectionStarted = 1;
#endif
		}
	}
	if(idx != -1) {
		if (g_listThread[idx].iDetectionStarted){
			AllocList_Add(&g_listAllocEntryList[idx], ptr, size, thread, g_iAllocNumber);
		}
		g_listThread[idx].iCurrentSize += size;
		g_listThread[idx].iAllocCount++;
		if(g_listThread[idx].iCurrentSize > g_listThread[idx].iMaxSize) {
			g_listThread[idx].iMaxSize = g_listThread[idx].iCurrentSize;
			if(g_iEnablePrint) {
				//ThreadEntry_print(&g_listThread[idx], size);
			}
		}
		if(displayEntry(&g_listThread[idx], size)){
			fprintf(stderr, "[aleakd] thread %lu (%s): %s %p (%lu bytes) => alloc=%d, memory=%lu bytes\n",
		   		g_listThread[idx].thread, (g_listThread[idx].name ? g_listThread[idx].name : ""),
				szAction, ptr, size,
				g_listThread[idx].iAllocCount, g_listThread[idx].iCurrentSize);
			if(size==49){
				fprintf(stderr, "[aleakd] leak detected\n");
			}
		}
	}
}

void removeEntry(void* ptr, char* szAction)
{
	int iFound = 0;
	size_t iSizeFree = 0;
	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdx(g_listThread, MAX_THREAD_COUNT, thread);
	if(idx != -1) {
		if (g_listThread[idx].iDetectionStarted) {
			iFound = AllocList_Remove(&g_listAllocEntryList[idx], ptr, &iSizeFree);
			if(iFound){
				g_listThread[idx].iCurrentSize-=iSizeFree;
				g_listThread[idx].iAllocCount--;
				if(displayEntry(&g_listThread[idx], iSizeFree)){
					fprintf(stderr, "[aleakd] thread %lu (%s): %s %p (%lu bytes) => alloc=%d, memory=%lu bytes\n",
							g_listThread[idx].thread, (g_listThread[idx].name ? g_listThread[idx].name : ""),
							szAction, ptr, iSizeFree,
							g_listThread[idx].iAllocCount, g_listThread[idx].iCurrentSize);
				}
			}
		}
	}
	// Look in other thread if not found
	if(iFound == 0){
		for(int i=0; i<MAX_THREAD_COUNT; i++){
			if ((i != idx) && g_listThread[i].iDetectionStarted) {
				iFound = AllocList_Remove(&g_listAllocEntryList[i], ptr, &iSizeFree);
				if(iFound){
					g_listThread[idx].iCurrentSize-=iSizeFree;
					g_listThread[i].iAllocCount--;
					if(displayEntry(&g_listThread[i], iSizeFree)){
						fprintf(stderr, "[aleakd] thread::fallback %lu (%s): %s %p (%lu bytes) => alloc=%d, memory=%lu bytes\n",
		   					 g_listThread[i].thread, (g_listThread[i].name ? g_listThread[i].name : ""),
		   					 szAction, ptr, iSizeFree,
		   					 g_listThread[i].iAllocCount, g_listThread[i].iCurrentSize);
					}
				}
			}
		}
	}
}


void *malloc(size_t size)
{
	if(real_malloc==NULL) {
		mtrace_init();
	}

	g_iAllocNumber++;

	void *p = NULL;
	p = real_malloc(size);

	if(p){
		addEntry(p, size, "malloc");
	}
	return p;
}


void *realloc(void* ptr, size_t size)
{
	if(real_realloc==NULL) {
		mtrace_init();
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
	if(real_free==NULL) {
		mtrace_init();
	}

	if(ptr != NULL) {
		removeEntry(ptr, "free");
	}

	//fprintf(stderr, "free => %p\n", ptr);
	real_free(ptr);
}
