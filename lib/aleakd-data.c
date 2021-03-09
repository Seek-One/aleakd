#include <string.h>

#include "../config.h"

#include "aleakd-data.h"

static unsigned long g_iAllocNumber = 0;
static unsigned long g_iBreakAllocNumber = 0;
static unsigned long g_iDisplayMinAllocNumber = 0;
static int g_bPrintAction = 0;

static char g_scanThread = 'm';

static struct ThreadEntry g_tabThreadEntry[MAX_THREAD_COUNT];
static struct ThreadEntryList g_listThreadEntry;

static struct AllocEntry g_tabAllocEntry[MAX_ALLOC_COUNT];
static struct AllocEntryList g_listAllocEntry;

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

void aleakd_data_set_enable_print_action(int bEnable)
{
	g_bPrintAction = bEnable;
}

int aleakd_data_get_enable_print_action()
{
	return g_bPrintAction;
}

void aleakd_data_init_alloc_list()
{
	memset(&g_tabAllocEntry, 0, sizeof(g_tabAllocEntry));
	g_listAllocEntry.max_count = MAX_ALLOC_COUNT;
	g_listAllocEntry.list = (struct AllocEntry*)&g_tabAllocEntry;
	g_listAllocEntry.last_alloc_num = 0;
	AllocList_Clear(&g_listAllocEntry);
}

void aleakd_data_init_thread_list()
{
	memset(&g_tabThreadEntry, 0, sizeof(g_tabThreadEntry));
	g_listThreadEntry.count = MAX_THREAD_COUNT;
	g_listThreadEntry.list = (struct ThreadEntry*)g_tabThreadEntry;

	// Free store data
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
	{
		// Init thread
		struct ThreadEntry* pThreadEntry = aleakd_data_get_thread(i);
		ThreadEntry_Reset(pThreadEntry);
#ifdef USE_AUTO_THREAD_START
		//printf("mymalloc started\n");
			pThreadEntry->iDetectionStarted = 1;
#endif
	}
}

struct ThreadEntryList* aleakd_data_get_thread_list()
{
	return (struct ThreadEntryList*)&g_listThreadEntry;
}

struct ThreadEntry* aleakd_data_get_thread(int idx)
{
	return ThreadEntry_getByIdx(&g_listThreadEntry, idx);
}

struct AllocEntryList* aleakd_data_get_alloc_list()
{
	return (struct AllocEntryList*)&g_listAllocEntry;
}

void aleakd_data_incr_alloc_number()
{
	g_iAllocNumber++;
}

unsigned long aleakd_data_get_alloc_number()
{
	return g_iAllocNumber;
}

void aleakd_data_set_break_alloc_num(unsigned long num)
{
	g_iBreakAllocNumber = num;
}

unsigned long aleakd_data_get_break_alloc_num()
{
	return g_iBreakAllocNumber;
}

void aleakd_data_set_display_min_alloc_num(unsigned long num)
{
	g_iDisplayMinAllocNumber = num;
}

unsigned long aleakd_data_get_display_min_alloc_num()
{
	return g_iDisplayMinAllocNumber;
}

void aleakd_data_lock()
{
	pthread_mutex_lock(&g_lock);
}

unsigned long aleakd_data_unlock()
{
	pthread_mutex_unlock(&g_lock);
}