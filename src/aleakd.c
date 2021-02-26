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


void aleakd_enable_print_action(int bEnable)
{
	aleakd_data_set_enable_print_action(bEnable);
}

void aleakd_all_threads_start()
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	for(int i=0; i<pThreadEntryList->count; i++)
	{
		pThreadEntryList->list[i].iDetectionStarted = 1;
	}
}

void aleakd_all_threads_stop()
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	for(int i=0; i<pThreadEntryList->count; i++)
	{
		pThreadEntryList->list[i].iDetectionStarted = 0;
	}
}

void aleakd_all_threads_print_leaks(int bDetail)
{
	AllocList_PrintLeaks_All(aleakd_data_get_alloc_list(), bDetail, aleakd_data_get_display_min_alloc_num());
}

void aleakd_all_threads_print_leaks_summary(int bDetail)
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	ThreadEntryList_PrintLeaks_Summary(pThreadEntryList);
}

int aleakd_all_threads_count()
{
	return MAX_THREAD_COUNT;
}

void aleakd_init(int idx)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->name = NULL;
	pThreadEntry->thread = 0;

	//aleakd_data_get_thread_list();
	//AllocList_Clear(&pThreadEntry->alloc_list);
}

void aleakd_set_current_thread(int idx, const char* name)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->name = name;
	pThreadEntry->thread = pthread_self();
}

void aleakd_current_thread_set_name(const char* name)
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdxAdd(pThreadEntryList, thread, MAX_ALLOC_COUNT);
	if(idx != -1){
		struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
#ifdef USE_AUTO_THREAD_START
		//printf("mymalloc started\n");
		pThreadEntry->iDetectionStarted = 1;
#endif
		pThreadEntry->name = name;
	}
}

void aleakd_current_thread_start()
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdxAdd(pThreadEntryList, thread, MAX_ALLOC_COUNT);
	if(idx != -1) {
		struct ThreadEntry *pThreadEntry = aleakd_data_get_thread(idx);
		pThreadEntry->iDetectionStarted = 1;
	}
}

void aleakd_current_thread_stop()
{
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdx(pThreadEntryList, thread);
	if(idx != -1) {
		struct ThreadEntry *pThreadEntry = aleakd_data_get_thread(idx);
		pThreadEntry->iDetectionStarted = 0;
	}
}

void aleakd_current_thread_print_leaks(int bDetail)
{
	pthread_t thread = pthread_self();
	AllocList_PrintLeaks_ForThread(aleakd_data_get_alloc_list(), bDetail, thread, aleakd_data_get_display_min_alloc_num());
}

void aleakd_start(int idx)
{
#ifdef USE_AUTO_THREAD_START
	//printf("mymalloc started\n");
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->iDetectionStarted = 1;
#endif
}

void aleakd_stop(int idx)
{
#ifndef USE_AUTO_THREAD_START
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->iDetectionStarted = 0;
	//printf("mymalloc stopped\n");
#endif
}

void aleakd_reset(int idx)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	ThreadEntry_Reinit(pThreadEntry);
}

void aleakd_thread_print_leaks(int idx, int bDetail)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	AllocList_PrintLeaks_ForThread(aleakd_data_get_alloc_list(), bDetail, pThreadEntry->thread, aleakd_data_get_display_min_alloc_num());
}

int aleakd_thread_alloc_count(int idx)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	return pThreadEntry->iAllocCount;
}

size_t aleakd_thread_memory_usage(int idx)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	return pThreadEntry->iCurrentSize;
}

size_t aleakd_thread_memory_usage_max(int idx)
{
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	return pThreadEntry->iMaxSize;
}

void aleakd_set_break_alloc_num(unsigned long num)
{
	aleakd_data_set_break_alloc_num(num);
}

void __attribute__((constructor)) aleakd_constructor()
{
	fprintf(stderr, "[aleakd] init\n");
	wrapper_init();
}

void __attribute__((destructor)) aleakd_destructor()
{
	fprintf(stderr, "[aleakd] dispose\n");
	AllocList_PrintLeaks_All(aleakd_data_get_alloc_list(), 0, aleakd_data_get_display_min_alloc_num());
	ThreadEntryList_PrintLeaks_Summary(aleakd_data_get_thread_list());
}
