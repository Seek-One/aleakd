#ifndef ALEAKD_THREADENTRY_H
#define ALEAKD_THREADENTRY_H

#include <pthread.h>

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

#endif // ALEAKD_THREADENTRY_H
