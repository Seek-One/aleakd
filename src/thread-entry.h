#ifndef ALEAKD_THREADENTRY_H
#define ALEAKD_THREADENTRY_H

#include <pthread.h>

#include "alloc-entry.h"

struct ThreadEntry
{
	const char* name;
	pthread_t thread;
	int iDetectionStarted;
	int iTabSize;
	size_t iCurrentSize;
	size_t iMaxSize;
	int iAllocCount;

	pthread_key_t key;

	struct AllocEntryList alloc_list;
} _ThreadEntry;

struct ThreadEntryList {
	struct ThreadEntry* list;
	int count;
} _ThreadEntryList;

int ThreadEntry_Reset(struct ThreadEntry* pThread);
int ThreadEntry_Reinit(struct ThreadEntry* pThread);
void ThreadEntry_Print(struct ThreadEntry* pThread, int bPrintDetail, unsigned long min_alloc_num);

struct ThreadEntry* ThreadEntry_getByIdx(struct ThreadEntryList* pThreadEntryList, int idx);
int ThreadEntry_getIdx(struct ThreadEntryList* pEntryList, pthread_t thread);
int ThreadEntry_getIdxAdd(struct ThreadEntryList* pEntryList, pthread_t thread, int iTabSize);

void ThreadEntryList_Print(struct ThreadEntryList* pEntryList, int bPrintDetail, unsigned long min_alloc_num);

#endif // ALEAKD_THREADENTRY_H
