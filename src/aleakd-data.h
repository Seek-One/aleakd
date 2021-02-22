#ifndef ALEAKD_DATA
#define ALEAKD_DATA

#include "../config.h"

#include "alloc-entry.h"
#include "thread-entry.h"

static unsigned long g_iAllocNumber;
static int g_iEnablePrint;

static char g_scanThread;

static struct ThreadEntry g_listThread[MAX_THREAD_COUNT];
static struct AllocEntry g_listAllocEntry[MAX_THREAD_COUNT][TAB_SIZE];
static struct AllocEntryList g_listAllocEntryList[MAX_THREAD_COUNT];

#endif // ALEAKD_DATA
