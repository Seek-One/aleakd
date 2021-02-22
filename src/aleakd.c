/*
 * mymalloc.c - Examples of run-time, link-time, and compile-time
 *              library interposition.
 */

#include <pthread.h>
#include <bits/pthreadtypes.h>

#include "../config.h"

#include "alloc-entry.h"

#include "aleakd.h"

static unsigned long g_iAllocNumber = 0;
static int g_iEnablePrint = 0;

char g_scanThread = 'm';

static struct ThreadEntry g_listThread[MAX_THREAD_COUNT];
static struct AllocEntry g_listAllocEntry[MAX_THREAD_COUNT][TAB_SIZE];
static struct AllocEntryList g_listAllocEntryList[MAX_THREAD_COUNT];

//#define USE_THREAD_START

#ifndef RUNTIME
#define RUNTIME
#endif

#ifdef RUNTIME
/*
 * Run-time interposition of malloc and free based
 * on the dynamic linker's (ld-linux.so) LD_PRELOAD mechanism
 *
 * Example (Assume a.out calls malloc and free):
 *   linux> gcc -O2 -Wall -o mymalloc.so -shared mymalloc.c
 *
 *   tcsh> setenv LD_PRELOAD "/usr/lib/libdl.so ./mymalloc.so"
 *   tcsh> ./a.out
 *   tcsh> unsetenv LD_PRELOAD
 *
 *   ...or
 *
 *   bash> (LD_PRELOAD="/usr/lib/libdl.so ./mymalloc.so" ./a.out)
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static void* (*real_malloc)(size_t)=NULL;
static void (*real_free)(void*)=NULL;

static void mtrace_init(void)
{
	real_malloc = dlsym(RTLD_NEXT, "malloc");
	if (NULL == real_malloc) {
		fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
	}

	real_free = dlsym(RTLD_NEXT, "free");
	if (NULL == real_malloc) {
		fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
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

void *malloc(size_t size)
{
	if(real_malloc==NULL) {
		mtrace_init();
	}

	g_iAllocNumber++;

	void *p = NULL;
	p = real_malloc(size);

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdx(g_listThread, MAX_THREAD_COUNT, thread);
	if(idx == -1) {
		idx = ThreadEntry_getIdxAdd(g_listThread, MAX_THREAD_COUNT, thread, TAB_SIZE);
		if(idx != -1) {
#ifndef USE_THREAD_START
			//printf("mymalloc started\n");
			g_listThread[idx].iDetectionStarted = 1;
#endif
		}
	}
	if(idx != -1) {
		if (g_listThread[idx].iDetectionStarted){
			AllocList_Add(&g_listAllocEntryList[idx], p, size, thread, g_iAllocNumber);
		}
		g_listThread[idx].iCurrentSize += size;
		g_listThread[idx].iAllocCount++;
		if(g_listThread[idx].iCurrentSize > g_listThread[idx].iMaxSize) {
			g_listThread[idx].iMaxSize = g_listThread[idx].iCurrentSize;
			if(g_iEnablePrint) {
				//ThreadEntry_print(&g_listThread[idx], size);
			}
		}
		if(g_listThread[idx].name && g_listThread[idx].name[0] == g_scanThread){
			fprintf(stderr, "[thread %lu = %s] malloc %p (%lu bytes) => alloc=%d, memory=%lu bytes\n",
		   		g_listThread[idx].thread, g_listThread[idx].name,
				p, size,
				g_listThread[idx].iAllocCount, g_listThread[idx].iCurrentSize);
			if(size==128 && g_listThread[idx].iAllocCount==411){
				fprintf(stderr, "[leak]");
			}
		}
	}

	return p;
}

void free(void *ptr)
{
	if(real_free==NULL) {
		mtrace_init();
	}

	if(ptr != NULL) {
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
					if(g_listThread[idx].name && g_listThread[idx].name[0] == g_scanThread){
						fprintf(stderr, "[thread %lu = %s] free %p (%lu bytes) => alloc=%d, memory=%lu bytes\n",
								g_listThread[idx].thread, g_listThread[idx].name,
								ptr, iSizeFree,
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
						if(g_listThread[i].name && g_listThread[i].name[0] == g_scanThread){
							fprintf(stderr, "[thread::fallback %lu = %s] free %p (%lu bytes) => alloc=%d, memory=%lu bytes\n",
			   					 g_listThread[i].thread, g_listThread[i].name,
			   					 ptr, iSizeFree,
			   					 g_listThread[i].iAllocCount, g_listThread[i].iCurrentSize);
						}
					}
				}
			}
		}
	}

	//fprintf(stderr, "free => %p\n", ptr);
	real_free(ptr);
}
#endif


#ifdef LINKTIME
/*
 * Link-time interposition of malloc and free using the static
 * linker's (ld) "--wrap symbol" flag.
 *
 * Compile the executable using "-Wl,--wrap,malloc -Wl,--wrap,free".
 * This tells the linker to resolve references to malloc as
 * __wrap_malloc, free as __wrap_free, __real_malloc as malloc, and
 * __real_free as free.
 */
#include <stdio.h>

void *__real_malloc(size_t size);
void __real_free(void *ptr);


/*
 * __wrap_malloc - malloc wrapper function
 */
void *__wrap_malloc(size_t size)
{
    void *ptr = __real_malloc(size);
    printf("malloc(%d) = %p\n", size, ptr);
    return ptr;
}

/*
 * __wrap_free - free wrapper function
 */
void __wrap_free(void *ptr)
{
    __real_free(ptr);
    printf("free(%p)\n", ptr);
}
#endif


#ifdef COMPILETIME
/*
 * Compile-time interposition of malloc and free using C
 * preprocessor. A local malloc.h file defines malloc (free) as
 * wrappers mymalloc (myfree) respectively.
 */

#include <stdio.h>
#include <malloc.h>

/*
 * mymalloc - malloc wrapper function
 */
void *mymalloc(size_t size, char *file, int line)
{
    void *ptr = malloc(size);
    printf("%s:%d: malloc(%d)=%p\n", file, line, size, ptr);
    return ptr;
}

/*
 * myfree - free wrapper function
 */
void myfree(void *ptr, char *file, int line)
{
    free(ptr);
    printf("%s:%d: free(%p)\n", file, line, ptr);
}
#endif

void  alkead_allow_print()
{
	g_iEnablePrint = 1;
}

void  alkead_init(int idx)
{
	g_listThread[idx].name = NULL;
	g_listThread[idx].thread = 0;
	AllocList_Reset(&g_listAllocEntryList[idx]);
}

void  alkead_set_current_thread(int idx, const char* name)
{
	g_listThread[idx].name = name;
	g_listThread[idx].thread = pthread_self();
	g_listAllocEntryList[idx].name = name;
}

void  alkead_set_current_thread_name(const char* name)
{
	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdxAdd(g_listThread, MAX_THREAD_COUNT, thread, TAB_SIZE);
	if(idx != -1){
#ifndef USE_THREAD_START
		//printf("mymalloc started\n");
		g_listThread[idx].iDetectionStarted = 1;
#endif
		g_listThread[idx].name = name;
	}
}

void  alkead_start(int idx)
{
#ifdef USE_THREAD_START
	//printf("mymalloc started\n");
	g_listThread[idx].iDetectionStarted = 1;
#endif
}

void  alkead_stop(int idx)
{
#ifdef USE_THREAD_START
	g_listThread[idx].iDetectionStarted = 0;
	//printf("mymalloc stopped\n");
#endif
}

void  alkead_reset(int idx)
{
	AllocList_Reset(&g_listAllocEntryList[idx]);
}

void  alkead_print_leak(int idx)
{
	AllocList_Print(&g_listAllocEntryList[idx]);
}
