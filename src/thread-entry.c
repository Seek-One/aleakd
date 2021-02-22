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

int ThreadEntry_getIdx(struct ThreadEntry* pEntryList, int iCount, pthread_t thread)
{
	for(int i=0; i<iCount; i++)
	{
		if(pEntryList[i].thread == thread){
			return i;
		}
	}
	return -1;
}

int ThreadEntry_getIdxAdd(struct ThreadEntry* pEntryList, int iCount, pthread_t thread, int iTabSize)
{
	for(int i=0; i<iCount; i++)
	{
		if(pEntryList[i].thread == thread){
			return i;
		}
	}
	for(int i=0; i<iCount; i++)
	{
		if(pEntryList[i].thread == 0){
			ThreadEntry_Reset(&pEntryList[i]);
			pEntryList[i].thread = thread;
			pEntryList[i].iTabSize = iTabSize;
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
