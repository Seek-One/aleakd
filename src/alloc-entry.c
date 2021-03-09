//
// Created by ebeuque on 18/02/2021.
//

#include <stdio.h>

#include "alloc-entry.h"

void AllocList_Clear(struct AllocEntryList* pEntryList)
{
	struct AllocEntry* pAllocEntry;
	pEntryList->count = 0;
	pEntryList->total_size = 0;
	for (int i=0; i<pEntryList->max_count; i++) {
		pAllocEntry = AllocList_getByIdx(pEntryList, i);
		pAllocEntry->ptr = NULL;
		pAllocEntry->thread = 0;
	}
}

void AllocList_Add(struct AllocEntryList* pEntryList, struct AllocEntry* pEntry)
{
	struct AllocEntry* pAllocEntry;
	for(int i=0; i<pEntryList->max_count; i++)
	{
		pAllocEntry = AllocList_getByIdx(pEntryList, i);
		if(pAllocEntry->ptr == NULL){
			pAllocEntry->ptr = pEntry->ptr;
			pAllocEntry->thread = pEntry->thread;
			pAllocEntry->size = pEntry->size;
			pAllocEntry->alloc_num = pEntry->alloc_num;
			pEntryList->count++;
			pEntryList->total_size += pEntry->size;

			//if(pEntry->size == 15 && pEntryList->name && pEntryList->name[0] == 'p') {
			//	fprintf(stderr, "[aleakd] thread %lu: malloc(%ld) #%d => %p\n", pEntry->thread, pEntry->size, pEntry->alloc_num, pEntry->ptr);
			//}

			return;
		}
	}

	fprintf(stderr, "[aleakd] thread %lu: no more space\n", pEntry->thread);
}

int AllocList_Remove(struct AllocEntryList* pEntryList, void* ptr, struct AllocEntry* pOutAllocEntry)
{
	int iFound = 0;
	int idx = -1;

	struct AllocEntry* pAllocEntry;
	for (int i = 0; i < pEntryList->max_count; i++)
	{
		pAllocEntry = AllocList_getByIdx(pEntryList, i);
		if (pAllocEntry->ptr == ptr)
		{
			// Copy info of suppressed entry
			if(pOutAllocEntry){
				pOutAllocEntry->ptr = pAllocEntry->ptr;
				pOutAllocEntry->thread = pAllocEntry->thread;
				pOutAllocEntry->size = pAllocEntry->size;
				pOutAllocEntry->alloc_num = pAllocEntry->alloc_num;
			}

			// Decrement list
			pEntryList->total_size -= pAllocEntry->size;
			pEntryList->count--;

			// Reset entry
			pAllocEntry->ptr = NULL;
			pAllocEntry->thread = 0;
			pAllocEntry->size = 0;
			pAllocEntry->alloc_num = 0;

			iFound = 1;
			idx = i;
			break;
		}
	}

	if(iFound){
		struct AllocEntry* pAllocEntry1 = NULL;
		struct AllocEntry* pAllocEntry2 = NULL;
		for (int i = idx; i < pEntryList->max_count-1; i++)
		{
			pAllocEntry1 = AllocList_getByIdx(pEntryList, i);
			pAllocEntry2 = AllocList_getByIdx(pEntryList, i+1);
			if (pAllocEntry1 && pAllocEntry2)
			{
				pAllocEntry1->ptr = pAllocEntry2->ptr;
				pAllocEntry1->thread = pAllocEntry2->thread;
				pAllocEntry1->size = pAllocEntry2->size;
				pAllocEntry1->alloc_num = pAllocEntry2->alloc_num;

				pAllocEntry2->ptr = NULL;
				pAllocEntry2->thread = 0;
				pAllocEntry2->size = 0;
				pAllocEntry2->alloc_num = 0;
			}
		}
	}


	return iFound;
}

struct AllocEntry* AllocList_getByIdx(struct AllocEntryList* pAllocEntryList, int idx)
{
	return &pAllocEntryList->list[idx];
}


void AllocEntryListPrintFilter_Reset(struct AllocEntryListPrintFilter* pFilter)
{
	pFilter->thread = 0;
	pFilter->iLimit = 0;
	pFilter->bLastest = 0;
	pFilter->iMinAlloc = 0;
	pFilter->bDetail = 0;
	pFilter->bNoMainThread = 0;
}

static void AllocList_PrintLeaks_Internal(struct AllocEntryList* pEntryList, struct AllocEntryListPrintFilter* pFilter)
{
	int iVisibleCount = 0;
	size_t iVisibleSize = 0;

	int bPrint;

	if(pFilter->thread){
		fprintf(stderr, "[aleakd] leak summary for thread %lu\n", pFilter->thread);
	}else{
		fprintf(stderr, "[aleakd] leak summary\n");
	}

	struct AllocEntry* pAllocEntry;
	if(pEntryList->count > 0)
	{
		pthread_t threadMain;
		pAllocEntry = AllocList_getByIdx(pEntryList, 0);
		if(pAllocEntry) {
			threadMain = pAllocEntry->thread;
		}

		for (int i = 0; i < pEntryList->max_count; i++)
		{
			pAllocEntry = AllocList_getByIdx(pEntryList, i);

			bPrint = 1;
			if(pAllocEntry->ptr == NULL){
				bPrint = 0;
			}
			if(pAllocEntry->alloc_num < pFilter->iMinAlloc){
				bPrint = 0;
			}
			if(pFilter->thread != 0 && (pFilter->thread != pAllocEntry->thread)){
				bPrint = 0;
			}
			if(pFilter->bLastest && (pFilter->iLimit > 0))
			{
				if(i<(pEntryList->count - pFilter->iLimit))
				{
					bPrint = 0;
				}
			}
			if(pFilter->bNoMainThread){
				if(pAllocEntry->thread == threadMain)
				{
					bPrint = 0;
				}
			}

			if (bPrint && pFilter->bDetail)
			{
				fprintf(stderr, "[aleakd]   leak for thread %lu: leak %p, size=%lu, alloc_num=%d\n",
						pAllocEntry->thread,
						//(pEntryList->name ? pEntryList->name : ""),
						pAllocEntry->ptr,
						pAllocEntry->size,
						pAllocEntry->alloc_num
				);
				iVisibleCount++;
				iVisibleSize+=pAllocEntry->size;
			}

			if(pFilter->iLimit > 0 && iVisibleCount == pFilter->iLimit){
				break;
			}

		}
		fprintf(stderr, "[aleakd]   leak visible: count=%d, size=%ld bytes\n", iVisibleCount, pAllocEntry->size);
		fprintf(stderr, "[aleakd]   leak total: count=%d, size=%ld bytes\n", pEntryList->count, pEntryList->total_size);
	}
}

void AllocList_PrintLeaks_All(struct AllocEntryList* pEntryList, int bDetail, unsigned long min_alloc)
{
	struct AllocEntryListPrintFilter filter;
	AllocEntryListPrintFilter_Reset(&filter);
	filter.bDetail = bDetail;
	filter.iMinAlloc = min_alloc;
	AllocList_PrintLeaks_Internal(pEntryList, &filter);
}

void AllocList_PrintLeaks_ForThread(struct AllocEntryList* pEntryList, int bDetail, pthread_t thread, unsigned long min_alloc)
{
	struct AllocEntryListPrintFilter filter;
	AllocEntryListPrintFilter_Reset(&filter);
	filter.bDetail = bDetail;
	filter.thread = thread;
	filter.iMinAlloc = min_alloc;
	AllocList_PrintLeaks_Internal(pEntryList, &filter);
}

void AllocList_PrintLeaks_Last(struct AllocEntryList* pEntryList, int iCount)
{
	struct AllocEntryListPrintFilter filter;
	AllocEntryListPrintFilter_Reset(&filter);
	filter.bDetail = 1;
	filter.iLimit = iCount;
	filter.bLastest = 1;
	filter.bNoMainThread = 0;
	AllocList_PrintLeaks_Internal(pEntryList, &filter);
}