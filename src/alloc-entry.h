//
// Created by ebeuque on 18/02/2021.
//

#ifndef TOOLKIT_DEBUG_MYMALLOC_ALLOCENTRY_H
#define TOOLKIT_DEBUG_MYMALLOC_ALLOCENTRY_H

#include <pthread.h>

struct AllocEntry {
	void* ptr;
	pthread_t thread;
	size_t size;
	int alloc_num;
} _AllocEntry;

struct AllocEntryList {
	struct AllocEntry* list;
	int max_count;
	int count;
	size_t total_size;
	const char* name;
};

void AllocList_Reset(struct AllocEntryList* pEntryList);
void AllocList_Add(struct AllocEntryList* pEntryList, void* ptr, size_t size, pthread_t thread, int alloc_num);
int AllocList_Remove(struct AllocEntryList* pEntryList, void* ptr, size_t* bufsize);
void AllocList_Print(struct AllocEntryList* pEntryList);

struct ThreadEntry
{
	const char* name;
	pthread_t thread;
	int iDetectionStarted;
	int iTabSize;
	size_t iCurrentSize;
	size_t iMaxSize;
	int iAllocCount;
} _ThreadEntry;

int ThreadEntry_Reset(struct ThreadEntry* pThread);
int ThreadEntry_getIdx(struct ThreadEntry* pEntryList, int iCount, pthread_t thread);
int ThreadEntry_getIdxAdd(struct ThreadEntry* pEntryList, int iCount, pthread_t thread, int iTabSize);
void ThreadEntry_print(struct ThreadEntry* pThread, size_t size_added);

#endif //TOOLKIT_DEBUG_MYMALLOC_ALLOCENTRY_H
