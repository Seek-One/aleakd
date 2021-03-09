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

int ThreadEntry_Reinit(struct ThreadEntry* pThread)
{
	pThread->iCurrentSize = 0;
	pThread->iMaxSize = 0;
	pThread->iAllocCount = 0;
}

void ThreadEntry_Print_Internal(struct ThreadEntry* pThreadEntry, int bPrintDetail, unsigned long min_alloc_num, int* piVisibleCount, size_t* piVisibleSize)
{
	struct AllocEntry* pAllocEntry;

	int iVisibleCount = 0;
	size_t iVisibleSize = 0;

	/*
	if(pThreadEntry->alloc_list.count > 0)
	{
		if(pThreadEntry->name) {
			fprintf(stderr, "[aleakd] thread %lu (%s) leak summary\n", pThreadEntry->thread, (pThreadEntry->name ? pThreadEntry->name : ""));
		}else{
			fprintf(stderr, "[aleakd] thread %lu leak summary\n", pThreadEntry->thread);
		}
		for (int i = 0; i < pThreadEntry->alloc_list.max_count; i++)
		{
			pAllocEntry = AllocList_getByIdx(&pThreadEntry->alloc_list, i);
			if (pAllocEntry->ptr != NULL && pAllocEntry->alloc_num >= min_alloc_num) {
				if(bPrintDetail) {
					fprintf(stderr, "[aleakd]   leak %p, size=%lu, alloc_num=%d\n",
							pAllocEntry->thread,
							pAllocEntry->ptr,
							pAllocEntry->size,
							pAllocEntry->alloc_num
					);
				}
				iVisibleCount++;
				iVisibleSize+=pAllocEntry->size;
			}
		}
		fprintf(stderr, "[aleakd]   leak visible: count=%d, size=%ld bytes\n", iVisibleCount, iVisibleSize);
		fprintf(stderr, "[aleakd]   leak total: count=%d, size=%ld bytes\n", pThreadEntry->alloc_list.count, pThreadEntry->alloc_list.total_size);
	}

	if(piVisibleCount){
		*piVisibleCount += iVisibleCount;
	}
	if(piVisibleSize){
		*piVisibleSize += iVisibleSize;
	}
	 */
}

void ThreadEntry_Print(struct ThreadEntry* pThreadEntry, int bPrintDetail, unsigned long min_alloc_num)
{
	ThreadEntry_Print_Internal(pThreadEntry, bPrintDetail, min_alloc_num, NULL, NULL);
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

void ThreadEntryList_PrintLeaks_Summary(struct ThreadEntryList* pEntryList)
{
	int iTotalAllocCount = 0;
	size_t iTotalAllocSize = 0;

	int bPrint;

	fprintf(stderr, "[aleakd] leak summary for threads\n");

	struct ThreadEntry* pThreaEntry;
	if(pEntryList->count > 0) {
		for (int i = 0; i < pEntryList->count; i++)
		{
			pThreaEntry = ThreadEntry_getByIdx(pEntryList, i);

			if(pThreaEntry->thread == 0) {
				bPrint = 0;
			}else{
				bPrint = 1;
			}
			//if(pThreaEntry->iAllocCount == 0){
			//	bPrint = 0;
			//}

			if (bPrint) {
				fprintf(stderr, "[aleakd]   leak for thread %lu (%s): alloc_count=%d, size=%lu, max_size=%lu\n",
						pThreaEntry->thread,
						(pThreaEntry->name ? pThreaEntry->name : ""),
						pThreaEntry->iAllocCount,
						pThreaEntry->iCurrentSize,
						pThreaEntry->iMaxSize
				);
				iTotalAllocCount+=pThreaEntry->iAllocCount;
				iTotalAllocSize+=pThreaEntry->iCurrentSize;
			}
		}
		fprintf(stderr, "[aleakd]   leak total: count=%d, size=%ld bytes\n", iTotalAllocCount, iTotalAllocSize);
	}
}