#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <bits/pthreadtypes.h>
#include <dlfcn.h>

#include "../config.h"

#include "aleakd-data.h"
#include "aleakd.h"

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

		// Init alloc list
		aleakd_data_init_alloc_list();

		// Init thread list
		aleakd_data_init_thread_list();

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
	if(!aleakd_data_get_enable_print_action()){
		return 0;
	}
	if(pAllocEntry->alloc_num < aleakd_data_get_display_min_alloc_num())
	{
		//if(pThread->name && pThread->iAllocCount > 10){
		//	return 0;
		//}
		return 0;
	}
	return 1;
}

void addEntry(void* ptr, size_t size, char* szAction, int alloc_num)
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	struct ThreadEntry* pThreadEntry = NULL;

	int bMustAdd = 1;

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdx(pThreadEntryList, thread);
	if(idx == -1) {
		idx = ThreadEntry_getIdxAdd(pThreadEntryList, thread, MAX_ALLOC_COUNT);
	}
	if(idx != -1) {
		pThreadEntry = ThreadEntry_getByIdx(pThreadEntryList, idx);
#ifdef USE_AUTO_THREAD_START
		pThreadEntry->iDetectionStarted = 1;
#endif
	}
	if(pThreadEntry && !pThreadEntry->iDetectionStarted){
		bMustAdd = 0;
	}

	struct AllocEntry allocEntry;
	allocEntry.ptr = ptr;
	allocEntry.size = size;
	allocEntry.thread = thread;
	allocEntry.alloc_num = alloc_num;

	// Add allocation in the list
	if(bMustAdd){
		AllocList_Add(aleakd_data_get_alloc_list(), &allocEntry);
	}

	// Add thread in the list
	if(pThreadEntry) {
		pThreadEntry = ThreadEntry_getByIdx(pThreadEntryList, idx);
		if (pThreadEntry->iDetectionStarted)
		{
			pThreadEntry->iCurrentSize += size;
			pThreadEntry->iAllocCount++;
			if (pThreadEntry->iCurrentSize > pThreadEntry->iMaxSize) {
				if(aleakd_data_get_alloc_number() > 22065) {
					fprintf(stderr, "[aleakd] thread %lu (%s): max size increase  with alloc %lu\n",
							pThreadEntry->thread, (pThreadEntry->name ? pThreadEntry->name : ""),
							aleakd_data_get_alloc_number());
				}
				pThreadEntry->iMaxSize = pThreadEntry->iCurrentSize;
			}
		}
	}

	// Check if we can display alloc
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

void removeEntry(void* ptr, char* szAction)
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	struct ThreadEntry* pThreadEntry;

	struct AllocEntry allocEntry;

	// Remove allocation in the list
	int iFound = 0;
	iFound = AllocList_Remove(aleakd_data_get_alloc_list(), ptr, &allocEntry);

	// Update the thread data
	if(iFound){
		// Update thread data
		int idx = ThreadEntry_getIdx(pThreadEntryList, allocEntry.thread);
		if(idx != -1) {
			pThreadEntry = ThreadEntry_getByIdx(pThreadEntryList, idx);
		}

		if(pThreadEntry) {
			pThreadEntry->iCurrentSize -= allocEntry.size;
			pThreadEntry->iAllocCount--;
		}

		if(pThreadEntry->iAllocCount < 0){
			fprintf(stderr, "[aleakd] error");
		}

		if (displayEntry(pThreadEntry, &allocEntry)) {
			fprintf(stderr, "[aleakd] thread %lu (%s): %s %p (%lu bytes) => alloc_count=%d, memory=%lu bytes\n",
					pThreadEntry->thread, (pThreadEntry->name ? pThreadEntry->name : ""),
					szAction, ptr, allocEntry.size,
					pThreadEntry->iAllocCount, pThreadEntry->iCurrentSize);
		}
	}

}

void *malloc(size_t size)
{
	void *p = NULL;

	aleakd_data_incr_alloc_number();
	int alloc_num = aleakd_data_get_alloc_number();

	if(!real_malloc){
		wrapper_init();
	}

	if(real_malloc){
		p = real_malloc(size);
	}else{
		p = dummy_malloc(size);
	}

	if(p){
		addEntry(p, size, "malloc", alloc_num);
	}

	return p;
}

void *calloc(size_t num, size_t size)
{
	aleakd_data_incr_alloc_number();
	int alloc_num = aleakd_data_get_alloc_number();

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
		addEntry(p, size*num, "calloc", alloc_num);
	}

	return p;
}


void *realloc(void* ptr, size_t size)
{
	if(!real_realloc){
		wrapper_init();
	}

	aleakd_data_incr_alloc_number();
	int alloc_num = aleakd_data_get_alloc_number();

	if(ptr){	
		removeEntry(ptr, "realloc::free");
	}

	void *p = NULL;
	p = real_realloc(ptr, size);
	
	if(p){
		addEntry(p, size, "realloc", alloc_num);
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

static void thread_cleanup (void * data)
{
	struct ThreadEntry* pThreadEntry = (struct ThreadEntry* )data;
	fprintf(stderr, "[aleakd] thread %lu (%s): celanup\n",
			pThreadEntry->thread, (pThreadEntry->name ? pThreadEntry->name : ""));

	pthread_key_delete(pThreadEntry->key);
	//aleakd_print_leaks(idx);

}

int (*real_pthread_create)(pthread_t *, const pthread_attr_t *, void *(*) (void *), void *);
//nt (*real_pthread_join)(pthread_t thread, void **retval);
//void (*real_pthread_exit)(void *rval_ptr);
//int (*real_pthread_cancel)(pthread_t thread);

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start) (void *), void *arg)
{
	int res;

	fprintf(stderr, "[aleakd] creating thread\n");
	if (!real_pthread_create) {
		real_pthread_create = dlsym(RTLD_NEXT, "pthread_create");
	}
	res = real_pthread_create(thread, attr, start, arg);

	if(thread){
		struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
		struct ThreadEntry* pThreadEntry;
		int idx = ThreadEntry_getIdxAdd(pThreadEntryList, *thread, MAX_ALLOC_COUNT);
		if(idx != -1){
			pThreadEntry = ThreadEntry_getByIdx(pThreadEntryList, idx);
			pthread_key_create(&pThreadEntry->key, thread_cleanup);
			pthread_setspecific(pThreadEntry->key, pThreadEntry);
		}
	}

	return res;
}

/*
int pthread_join(pthread_t thread, void **retval)
{
	int res;

	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	struct ThreadEntry* pThreadEntry;
	int idx = ThreadEntry_getIdx(pThreadEntryList, thread);

	if (!real_pthread_join) {
		real_pthread_join = dlsym(RTLD_NEXT, "pthread_join");
	}
	res =  real_pthread_join(thread, retval);

	if(idx != -1){
		pThreadEntry = aleakd_data_get_thread(idx);
		fprintf(stderr, "[aleakd] thread %lu (%s): join\n",
			pThreadEntry->thread, (pThreadEntry->name ? pThreadEntry->name : ""));

		aleakd_print_leaks(idx);
	}

	return res;
}

void pthread_exit(void *rval_ptr)
{
	fprintf(stderr, "[aleakd] exit thread\n");
	if (!real_pthread_exit) {
		real_pthread_exit = dlsym(RTLD_NEXT, "pthread_exit");
	}

	real_pthread_exit(rval_ptr);
}

int pthread_cancel(pthread_t thread)
{
	int res;

	fprintf(stderr, "[aleakd] cancel thread\n");
	if (!real_pthread_cancel) {
		real_pthread_cancel = dlsym(RTLD_NEXT, "pthread_cancel");
	}

	return res;
}
*/