/*
 * mymalloc.c - Examples of run-time, link-time, and compile-time
 *              library interposition.
 */

#include <stdio.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>

#include "wrapper.h"

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


void aleakd_allow_print()
{
	aleakd_data_set_enable_print(1);
}

void aleakd_init(int idx)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->name = NULL;
	pThreadEntry->thread = 0;
	AllocList_Reset(&pThreadEntry->alloc_list);
}

void aleakd_set_current_thread(int idx, const char* name)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->name = name;
	pThreadEntry->thread = pthread_self();

	pThreadEntry->alloc_list.name = name;
}

void aleakd_set_current_thread_name(const char* name)
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdxAdd(pThreadEntryList, thread, TAB_SIZE);
	if(idx != -1){
		struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);

#ifndef USE_THREAD_START
		//printf("mymalloc started\n");
		pThreadEntry->iDetectionStarted = 1;
#endif
		pThreadEntry->name = name;
	}
}

void aleakd_start(int idx)
{
#ifdef USE_THREAD_START
	//printf("mymalloc started\n");
	g_listThread[idx].iDetectionStarted = 1;
#endif
}

void aleakd_stop(int idx)
{
#ifdef USE_THREAD_START
	g_listThread[idx].iDetectionStarted = 0;
	//printf("mymalloc stopped\n");
#endif
}

void aleakd_reset(int idx)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	AllocList_Reset(&pThreadEntry->alloc_list);
}

void aleakd_print_leak(int idx)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	AllocList_Print(&pThreadEntry->alloc_list);
}

void __attribute__((constructor)) aleakd_constructor()
{
	fprintf(stderr, "[aleakd] init\n");
	wrapper_init();
}

void __attribute__((destructor)) aleakd_destructor()
{
	fprintf(stderr, "[aleakd] dispose\n");

	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	struct ThreadEntry* pThreadEntry;
	for(int i=0; i<pThreadEntryList->count; i++)
	{
		pThreadEntry = &pThreadEntryList->list[i];
		AllocList_Print(&pThreadEntry->alloc_list);
	}
}
