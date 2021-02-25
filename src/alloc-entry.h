//
// Created by ebeuque on 18/02/2021.
//

#ifndef ALEAKD_ALLOCENTRY_H
#define ALEAKD_ALLOCENTRY_H

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
	int last_alloc_num;
	size_t total_size;
};

void AllocList_Clear(struct AllocEntryList* pEntryList);
void AllocList_Add(struct AllocEntryList* pEntryList, struct AllocEntry* pEntry);
int AllocList_Remove(struct AllocEntryList* pEntryList, void* ptr, struct AllocEntry* pEntry);
struct AllocEntry* AllocList_getByIdx(struct AllocEntryList* pAllocEntryList, int idx);

void AllocList_PrintLeaks_All(struct AllocEntryList* pEntryList, unsigned long min_alloc);
void AllocList_PrintLeaks_ForThread(struct AllocEntryList* pEntryList, pthread_t thread, unsigned long min_alloc);

#endif // ALEAKD_ALLOCENTRY_H
