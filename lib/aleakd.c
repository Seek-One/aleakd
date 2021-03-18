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
	aleakd_data_lock();
	aleakd_data_set_enable_print_action(bEnable);
	aleakd_data_unlock();
}

void aleakd_all_threads_start()
{
	aleakd_data_lock();
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	for(int i=0; i<pThreadEntryList->count; i++)
	{
		pThreadEntryList->list[i].iDetectionStarted = 1;
	}
	aleakd_data_unlock();
}

void aleakd_all_threads_stop()
{
	aleakd_data_lock();
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	for(int i=0; i<pThreadEntryList->count; i++)
	{
		pThreadEntryList->list[i].iDetectionStarted = 0;
	}
	aleakd_data_unlock();
}

void aleakd_all_threads_print_leaks(int bDetail)
{
	aleakd_data_lock();
	AllocList_PrintLeaks_All(aleakd_data_get_alloc_list(), bDetail, aleakd_data_get_display_min_alloc_num());
	aleakd_data_unlock();
}

void aleakd_all_threads_print_leaks_summary(int bDetail)
{
	aleakd_data_lock();
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();
	ThreadEntryList_PrintLeaks_Summary(pThreadEntryList);
	aleakd_data_unlock();
}

void aleakd_all_threads_print_last_leaks(int iCount)
{
	aleakd_data_lock();
	AllocList_PrintLeaks_Last(aleakd_data_get_alloc_list(), iCount);
	aleakd_data_unlock();
}

int aleakd_all_threads_count()
{
	return MAX_THREAD_COUNT;
}

void aleakd_init(int idx)
{
	aleakd_data_lock();
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->name = NULL;
	pThreadEntry->thread = 0;

	//aleakd_data_get_thread_list();
	//AllocList_Clear(&pThreadEntry->alloc_list);
	aleakd_data_unlock();
}

void aleakd_set_current_thread(int idx, const char* name)
{
	aleakd_data_lock();
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->name = name;
	pThreadEntry->thread = pthread_self();
	aleakd_data_unlock();
}

void aleakd_current_thread_set_name(const char* name)
{
	aleakd_data_lock();
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
	aleakd_data_unlock();
}

void aleakd_current_thread_start()
{
	aleakd_data_lock();
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdxAdd(pThreadEntryList, thread, MAX_ALLOC_COUNT);
	if(idx != -1) {
		struct ThreadEntry *pThreadEntry = aleakd_data_get_thread(idx);
		pThreadEntry->iDetectionStarted = 1;
	}
	aleakd_data_unlock();
}

void aleakd_current_thread_stop()
{
	aleakd_data_lock();
	struct ThreadEntryList* pThreadEntryList = aleakd_data_get_thread_list();

	pthread_t thread = pthread_self();
	int idx = ThreadEntry_getIdx(pThreadEntryList, thread);
	if(idx != -1) {
		struct ThreadEntry *pThreadEntry = aleakd_data_get_thread(idx);
		pThreadEntry->iDetectionStarted = 0;
	}
	aleakd_data_unlock();
}

void aleakd_current_thread_print_leaks(int bDetail)
{
	pthread_t thread = pthread_self();
	aleakd_data_lock();
	AllocList_PrintLeaks_ForThread(aleakd_data_get_alloc_list(), bDetail, thread, aleakd_data_get_display_min_alloc_num());
	aleakd_data_unlock();
}

void aleakd_start(int idx)
{
	aleakd_data_lock();
#ifdef USE_AUTO_THREAD_START
	//printf("mymalloc started\n");
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->iDetectionStarted = 1;
#endif
	aleakd_data_unlock();
}

void aleakd_stop(int idx)
{
	aleakd_data_lock();
#ifndef USE_AUTO_THREAD_START
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	pThreadEntry->iDetectionStarted = 0;
	//printf("mymalloc stopped\n");
#endif
	aleakd_data_unlock();
}

void aleakd_reset(int idx)
{
	aleakd_data_lock();
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	ThreadEntry_Reinit(pThreadEntry);
	aleakd_data_unlock();
}

void aleakd_thread_print_leaks(int idx, int bDetail)
{
	aleakd_data_lock();
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	AllocList_PrintLeaks_ForThread(aleakd_data_get_alloc_list(), bDetail, pThreadEntry->thread, aleakd_data_get_display_min_alloc_num());
	aleakd_data_unlock();
}

int aleakd_thread_alloc_count(int idx)
{
	size_t iResult;
	aleakd_data_lock();
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	iResult = pThreadEntry->iAllocCount;
	aleakd_data_unlock();
	return iResult;
}

size_t aleakd_thread_memory_usage(int idx)
{
	size_t iResult;
	aleakd_data_lock();
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	iResult = pThreadEntry->iCurrentSize;
	aleakd_data_unlock();
	return iResult;
}

size_t aleakd_thread_memory_usage_max(int idx)
{
	size_t iResult;
	aleakd_data_lock();
	struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(idx);
	iResult = pThreadEntry->iMaxSize;
	aleakd_data_unlock();
	return iResult;
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
	aleakd_data_lock();
	AllocList_PrintLeaks_Last(aleakd_data_get_alloc_list(), 0);
	AllocList_PrintLeaks_All(aleakd_data_get_alloc_list(), 0, aleakd_data_get_display_min_alloc_num());
	ThreadEntryList_PrintLeaks_Summary(aleakd_data_get_thread_list());
	aleakd_data_unlock();
	wrapper_dispose();
}
