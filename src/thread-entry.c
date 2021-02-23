#include <stdio.h>

#include "thread-entry.h"

int ThreadEntry_Reset(struct ThreadEntry* pThread)
{
	pThread->name = NULL;
	pThread->thread = 0;
	pThread->iDetectionStarted = 0;
	pThread->iTabSize = 0;
	pThread->iCurrentSize = 0;
	pThread->iMaxSize = 0;
	pThread->iAllocCount = 0;
}

struct ThreadEntry* ThreadEntry_getByIdx(struct ThreadEntryList* pThreadEntryList, int idx)
{
	return &pThreadEntryList->list[idx];
}

int ThreadEntry_getIdx(struct ThreadEntryList* pEntryList, pthread_t thread)
{
	struct ThreadEntry* pThreadEntry;
	for(int i=0; i<pEntryList->count; i++)
	{
		pThreadEntry = &pEntryList->list[i];
		if(pThreadEntry->thread == thread){
			return i;
		}
	}
	return -1;
}

int ThreadEntry_getIdxAdd(struct ThreadEntryList* pEntryList, pthread_t thread, int iTabSize)
{
	struct ThreadEntry* pThreadEntry;
	for(int i=0; i<pEntryList->count; i++)
	{
		pThreadEntry = &pEntryList->list[i];
		if(pThreadEntry->thread == thread){
			return i;
		}
	}
	for(int i=0; i<pEntryList->count; i++)
	{
		pThreadEntry = &pEntryList->list[i];
		if(pThreadEntry->thread == 0){
			ThreadEntry_Reset(pThreadEntry);
			pThreadEntry->thread = thread;
			pThreadEntry->iTabSize = iTabSize;
			return i;
		}
	}
	return -1;
}

void ThreadEntry_print(struct ThreadEntry* pThread, size_t size_added)
{
	fprintf(stderr, "thread #%lu '%s' : total=%ld bytes, alloc=%ld, added=%lu bytes)\n",
		 pThread->thread, (pThread->name ? pThread->name : ""), pThread->iMaxSize, pThread->iAllocCount, size_added);
}
