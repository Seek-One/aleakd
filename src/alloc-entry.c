//
// Created by ebeuque on 18/02/2021.
//

#include <stdio.h>

#include "alloc-entry.h"

void AllocList_Reset(struct AllocEntryList* pEntryList)
{
	struct AllocEntry* pAllocEntry;
	pEntryList->count = 0;
	for (int i=0; i<pEntryList->max_count; i++) {
		pAllocEntry = AllocList_getByIdx(pEntryList, i);
		pAllocEntry->ptr = NULL;
		pAllocEntry->thread = 0;
	}
}

void AllocList_Add(struct AllocEntryList* pEntryList, void* ptr, size_t size, pthread_t thread, int alloc_num)
{
	struct AllocEntry* pAllocEntry;
	for(int i=0; i<pEntryList->max_count; i++)
	{
		pAllocEntry = AllocList_getByIdx(pEntryList, i);
		if(pAllocEntry->ptr == NULL){
			pAllocEntry->ptr = ptr;
			pAllocEntry->thread = thread;
			pAllocEntry->size = size;
			pAllocEntry->alloc_num = alloc_num;
			pEntryList->count++;
			pEntryList->total_size += size;

			if(size == 15 && pEntryList->name && pEntryList->name[0] == 'p') {
				fprintf(stderr, "[aleakd] thread %lu: malloc(%ld) #%d => %p\n", thread, size, alloc_num, ptr);
			}

			return;
		}
	}

	fprintf(stderr, "[aleakd] thread %lu: no more space\n", thread);
}

int AllocList_Remove(struct AllocEntryList* pEntryList, void* ptr, size_t* bufsize)
{
	struct AllocEntry* pAllocEntry;
	for (int i = 0; i < pEntryList->max_count; i++)
	{
		pAllocEntry = AllocList_getByIdx(pEntryList, i);
		if (pAllocEntry->ptr == ptr)
		{
			pEntryList->total_size -= pAllocEntry->size;
			pEntryList->count--;
			pAllocEntry->ptr = NULL;
			pAllocEntry->thread = 0;
			if(bufsize){
				*bufsize = pAllocEntry->size;
			}
			pAllocEntry->size = 0;
			pAllocEntry->alloc_num = 0;
			return 1;
		}
	}
	return 0;
}

void AllocList_Print(struct AllocEntryList* pEntryList)
{
	struct AllocEntry* pAllocEntry;
	if(pEntryList->count > 0) {
		for (int i = 0; i < pEntryList->max_count; i++)
		{
			pAllocEntry = AllocList_getByIdx(pEntryList, i);
			if (pAllocEntry->ptr != NULL) {
				fprintf(stderr, "[aleakd] thread %lu (%s): leak %p, size=%lu, alloc_num=%d\n",
						pAllocEntry->thread,
						(pEntryList->name ? pEntryList->name : ""),
						pAllocEntry->ptr,
						pAllocEntry->size,
						pAllocEntry->alloc_num
				);
			}
		}
		fprintf(stderr, "[aleakd] leak total: count=%d, size=%ld bytes\n", pEntryList->count, pEntryList->total_size);
	}
}


struct AllocEntry* AllocList_getByIdx(struct AllocEntryList* pAllocEntryList, int idx)
{
	return &pAllocEntryList->list[idx];
}