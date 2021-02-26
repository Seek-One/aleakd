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

			return 1;
		}
	}
	return 0;
}

struct AllocEntry* AllocList_getByIdx(struct AllocEntryList* pAllocEntryList, int idx)
{
	return &pAllocEntryList->list[idx];
}

static void AllocList_PrintLeaks_Internal(struct AllocEntryList* pEntryList, int bDetail, pthread_t *pThread, unsigned long min_alloc)
{
	int iVisibleCount = 0;
	size_t iVisibleSize = 0;

	int bPrint;

	if(pThread) {
		fprintf(stderr, "[aleakd] leak summary for thread %lu\n", pThread);
	}else{
		fprintf(stderr, "[aleakd] leak summary\n");
	}

	struct AllocEntry* pAllocEntry;
	if(pEntryList->count > 0) {
		for (int i = 0; i < pEntryList->max_count; i++)
		{
			pAllocEntry = AllocList_getByIdx(pEntryList, i);

			bPrint = 1;
			if(pAllocEntry->ptr == NULL){
				bPrint = 0;
			}
			if(pAllocEntry->alloc_num < min_alloc){
				bPrint = 0;
			}
			if(pThread && (*pThread != pAllocEntry->thread)){
				bPrint = 0;
			}

			if (bPrint && bDetail) {
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
		}
		fprintf(stderr, "[aleakd]   leak visible: count=%d, size=%ld bytes\n", iVisibleCount, pAllocEntry->size);
		fprintf(stderr, "[aleakd]   leak total: count=%d, size=%ld bytes\n", pEntryList->count, pEntryList->total_size);
	}
}

void AllocList_PrintLeaks_All(struct AllocEntryList* pEntryList, int bDetail, unsigned long min_alloc)
{
	AllocList_PrintLeaks_Internal(pEntryList, bDetail, NULL, min_alloc);
}

void AllocList_PrintLeaks_ForThread(struct AllocEntryList* pEntryList, int bDetail, pthread_t thread, unsigned long min_alloc)
{
	AllocList_PrintLeaks_Internal(pEntryList, bDetail, &thread, min_alloc);
}