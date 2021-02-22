/*
 * mymalloc.c - Examples of run-time, link-time, and compile-time
 *              library interposition.
 */

#include <pthread.h>
#include <bits/pthreadtypes.h>

#include "aleakd-data.h"

#include "aleakd.h"

//#define USE_THREAD_START

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


void  aleakd_allow_print()
{
	g_iEnablePrint = 1;
}

void  aleakd_init(int idx)
{
	g_listThread[idx].name = NULL;
	g_listThread[idx].thread = 0;
	AllocList_Reset(&g_listAllocEntryList[idx]);
}

void  aleakd_set_current_thread(int idx, const char* name)
{
	g_listThread[idx].name = name;
	g_listThread[idx].thread = pthread_self();
	g_listAllocEntryList[idx].name = name;
}

void  aleakd_set_current_thread_name(const char* name)
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

void  aleakd_start(int idx)
{
#ifdef USE_THREAD_START
	//printf("mymalloc started\n");
	g_listThread[idx].iDetectionStarted = 1;
#endif
}

void  aleakd_stop(int idx)
{
#ifdef USE_THREAD_START
	g_listThread[idx].iDetectionStarted = 0;
	//printf("mymalloc stopped\n");
#endif
}

void  aleakd_reset(int idx)
{
	AllocList_Reset(&g_listAllocEntryList[idx]);
}

void  aleakd_print_leak(int idx)
{
	AllocList_Print(&g_listAllocEntryList[idx]);
}
