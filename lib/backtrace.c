//
// Created by ebeuque on 23/03/2021.
//
#define _GNU_SOURCE
#include <stdio.h>
#include <link.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "backtrace.h"

#define MAX_BUFF_ADDR 16384
static void* g_listAddr[MAX_BUFF_ADDR];
static int g_listAddrCount[MAX_BUFF_ADDR];

pthread_mutex_t g_lockAddrCount = PTHREAD_MUTEX_INITIALIZER;

void backtrace_init()
{
	memset(g_listAddr, 0, sizeof(g_listAddr));
	memset(g_listAddrCount, 0, sizeof(g_listAddrCount));

	void* listBacktraceAddr[BACKTRACE_MAX_SIZE];
	backtrace (listBacktraceAddr, BACKTRACE_MAX_SIZE);
}

void backtrace_get_infos(void* addr, const char** pfname, void** pfbase, const char** psfname, void** psaddr)
{
	Dl_info info;
	dladdr(addr, &info);
	if(pfname){
		*pfname = info.dli_fname;
	}
	if(pfbase){
		*pfbase = info.dli_fbase;
	}
	if(psfname){
		*psfname = info.dli_sname;
	}
	if(psaddr){
		*psaddr = info.dli_saddr;
	}
}

int backtrace_check_addr(void* addr)
{
	int iPosMin = 0;
	int iValMin = INT32_MAX;

	pthread_mutex_lock(&g_lockAddrCount);
	for(int i=0; i<MAX_BUFF_ADDR; i++)
	{
		if(g_listAddr[i] == addr){
			// Increment occurrence count
			g_listAddrCount[i] = g_listAddrCount[i] + 1;
			pthread_mutex_unlock(&g_lockAddrCount);
			return 1;
		}
		if(g_listAddrCount[i] < iValMin)
		{
			iPosMin = i;
			iValMin = g_listAddrCount[i];
		}
	}
	g_listAddr[iPosMin] = addr;
	g_listAddrCount[iPosMin] = 1;

	pthread_mutex_unlock(&g_lockAddrCount);

	return 0;
}

#if __ELF_NATIVE_CLASS == 32
# define WORD_WIDTH 8
#else
/* We assume 64bits.  */
# define WORD_WIDTH 16
#endif

void backtrace_symbols_print (void *const *array, int size)
{
	Dl_info info[size];
	int status[size];
	int cnt;
	size_t total = 0;
	/* Fill in the information we can get from `dladdr'.  */
	for (cnt = 0; cnt < size; ++cnt)
	{
		struct link_map *map;
		//status[cnt] = _dl_addr (array[cnt], &info[cnt], &map, NULL);
		status[cnt] = dladdr1 (array[cnt], &info[cnt], (void **)&map, RTLD_DL_LINKMAP);
		if (status[cnt] && info[cnt].dli_fname && info[cnt].dli_fname[0] != '\0')
		{
			/* We have some info, compute the length of the string which will be
			   "<file-name>(<sym-name>+offset) [address].  */
			total += (strlen (info[cnt].dli_fname ?: "")
					  + strlen (info[cnt].dli_sname ?: "")
					  + 3 + WORD_WIDTH + 3 + WORD_WIDTH + 5);
			/* The load bias is more useful to the user than the load
			   address.  The use of these addresses is to calculate an
			   address in the ELF file, so its prelinked bias is not
			   something we want to subtract out.  */
			info[cnt].dli_fbase = (void *) map->l_addr;
		}
		else
			total += 5 + WORD_WIDTH;
	}
	/* Allocate memory for the result.  */
	for (cnt = 0; cnt < size; ++cnt)
	{
		if (status[cnt]
			&& info[cnt].dli_fname != NULL && info[cnt].dli_fname[0] != '\0')
		{
			if (info[cnt].dli_sname == NULL) {
				/* We found no symbol name to use, so describe it as
				   relative to the file.  */
				info[cnt].dli_saddr = info[cnt].dli_fbase;
			}
			if (info[cnt].dli_sname == NULL && info[cnt].dli_saddr == 0) {
				fprintf(stderr, "%s(%s) [%p]\n",
									info[cnt].dli_fname ?: "",
									info[cnt].dli_sname ?: "",
									array[cnt]);
			}else{
				char sign;
				ptrdiff_t offset;
				if (array[cnt] >= (void *) info[cnt].dli_saddr)
				{
					sign = '+';
					offset = (ptrdiff_t)array[cnt] - (ptrdiff_t)info[cnt].dli_saddr;
				}
				else
				{
					sign = '-';
					offset = (ptrdiff_t)info[cnt].dli_saddr - (ptrdiff_t)array[cnt];
				}
				fprintf(stderr, "%s(%s%c%#tx) [%p]\n",
									 info[cnt].dli_fname ?: "",
									 info[cnt].dli_sname ?: "",
									 sign, offset, array[cnt]);
			}
		} else {
			fprintf(stderr, "[%p]\n", array[cnt]);
		}
	}
}

void backtrace_print()
{
	void* listBacktraceAddr[BACKTRACE_MAX_SIZE];
	int iBacktraceSize = backtrace (listBacktraceAddr, BACKTRACE_MAX_SIZE);
	fprintf(stderr, "backtrace\n");
	for(int i=0; i<iBacktraceSize; i++)
	{
		void* addr = listBacktraceAddr[i];
		Dl_info info;
		dladdr(addr, &info);
		fprintf(stderr, "  0x%p => %s (%s)\n", addr, info.dli_fname, info.dli_sname);
	}
}

void
backtrace_print2 ()
{
	void* listAddr[50];
	int iBtSize = backtrace(listAddr, 50);
	backtrace_symbols_print(listAddr, iBtSize);
}
