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

	struct AllocEntryList alloc_list;
} _ThreadEntry;

struct ThreadEntryList {
	struct ThreadEntry* list;
	int count;
} _ThreadEntryList;

int ThreadEntry_Reset(struct ThreadEntry* pThread);
void ThreadEntry_print(struct ThreadEntry* pThread, size_t size_added);

struct ThreadEntry* ThreadEntry_getByIdx(struct ThreadEntryList* pThreadEntryList, int idx);
int ThreadEntry_getIdx(struct ThreadEntryList* pEntryList, pthread_t thread);
int ThreadEntry_getIdxAdd(struct ThreadEntryList* pEntryList, pthread_t thread, int iTabSize);

#endif // ALEAKD_THREADENTRY_H
