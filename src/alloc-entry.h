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
	size_t total_size;
	const char* name;
};

void AllocList_Reset(struct AllocEntryList* pEntryList);
void AllocList_Add(struct AllocEntryList* pEntryList, void* ptr, size_t size, pthread_t thread, int alloc_num);
int AllocList_Remove(struct AllocEntryList* pEntryList, void* ptr, size_t* bufsize);
void AllocList_Print(struct AllocEntryList* pEntryList);

#endif // ALEAKD_ALLOCENTRY_H
