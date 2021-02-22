#include "../config.h"

#include "aleakd-data.h"

static unsigned long g_iAllocNumber = 0;
static int g_iEnablePrint = 0;

static char g_scanThread = 'm';

static struct ThreadEntry g_listThread[MAX_THREAD_COUNT];
static struct AllocEntry g_listAllocEntry[MAX_THREAD_COUNT][TAB_SIZE];
static struct AllocEntryList g_listAllocEntryList[MAX_THREAD_COUNT];
