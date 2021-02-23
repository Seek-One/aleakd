#include "../config.h"

#include "aleakd-data.h"

static unsigned long g_iAllocNumber = 0;
static int g_iEnablePrint = 0;

static char g_scanThread = 'm';

static struct ThreadEntry g_listThread[MAX_THREAD_COUNT];
static struct ThreadEntryList g_listThreadList;

static struct AllocEntry g_listAllocEntry[MAX_THREAD_COUNT][TAB_SIZE];

void aleakd_data_set_enable_print(int bEnable)
{
	g_iEnablePrint = bEnable;
}

int aleakd_data_get_enable_print()
{
	return g_iEnablePrint;
}

void aleakd_data_init_thread_list()
{
	g_listThreadList.count = MAX_THREAD_COUNT;
	g_listThreadList.list = (struct ThreadEntry*)g_listThread;
}

struct ThreadEntryList* aleakd_data_get_thread_list()
{
	return (struct ThreadEntryList*)&g_listThreadList;
}

struct ThreadEntry* aleakd_data_get_thread(int idx)
{
	return ThreadEntry_getByIdx(&g_listThreadList, idx);
}

struct AllocEntry* aleakd_data_get_alloc_list(int idx)
{
	return (struct AllocEntry*)g_listAllocEntry[idx];
}

void aleakd_data_incr_alloc_number()
{
	g_iAllocNumber++;
}

int aleakd_data_get_alloc_number()
{
	return g_iAllocNumber;
}