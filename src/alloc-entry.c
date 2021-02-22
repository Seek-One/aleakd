//
// Created by ebeuque on 18/02/2021.
//

#include <stdio.h>

#include "alloc-entry.h"

void AllocList_Reset(struct AllocEntryList* pEntryList)
{
	pEntryList->count = 0;
	for (int i=0; i<pEntryList->max_count; i++) {
		pEntryList->list[i].ptr = NULL;
		pEntryList->list[i].thread = 0;
	}
}

void AllocList_Add(struct AllocEntryList* pEntryList, void* ptr, size_t size, pthread_t thread, int alloc_num)
{
	for(int i=0; i<pEntryList->max_count; i++){
		if(pEntryList->list[i].ptr == NULL){
			pEntryList->list[i].ptr = ptr;
			pEntryList->list[i].thread = thread;
			pEntryList->list[i].size = size;
			pEntryList->list[i].alloc_num = alloc_num;
			pEntryList->count++;
			pEntryList->total_size += size;

			if(size == 15 && pEntryList->name && pEntryList->name[0] == 'p') {
				fprintf(stderr, "thread:%lu malloc(%ld) #%d => %p\n", thread, size, alloc_num, ptr);
			}

			return;
		}
	}

	fprintf(stderr, "thread:%lu no more space\n", thread);
}

int AllocList_Remove(struct AllocEntryList* pEntryList, void* ptr, size_t* bufsize)
{
	for (int i = 0; i < pEntryList->max_count; i++)
	{
		if (pEntryList->list[i].ptr == ptr)
		{
			pEntryList->total_size -= pEntryList->list[i].size;
			pEntryList->count--;
			pEntryList->list[i].ptr = NULL;
			pEntryList->list[i].thread = 0;
			if(bufsize){
				*bufsize = pEntryList->list[i].size;
			}
			pEntryList->list[i].size = 0;
			pEntryList->list[i].alloc_num = 0;
			return 1;
		}
	}
	return 0;
}

void AllocList_Print(struct AllocEntryList* pEntryList)
{
	if(pEntryList->count > 0) {
		for (int i = 0; i < pEntryList->max_count; i++) {
			if (pEntryList->list[i].ptr != NULL) {
				fprintf(stderr, "still => %p (thread=%lu(%s), size=%lu, num=%d)\n",
						pEntryList->list[i].ptr,
						pEntryList->list[i].thread,
						(pEntryList->name ? pEntryList->name : ""),
						pEntryList->list[i].size,
						pEntryList->list[i].alloc_num
				);
			}
		}
		fprintf(stderr, "count => %d (%ld bytes)\n", pEntryList->count, pEntryList->total_size);
	}
}

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
