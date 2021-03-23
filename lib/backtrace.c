//
// Created by ebeuque on 23/03/2021.
//
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

#include "backtrace.h"

void backtrace_init()
{
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

void backtrace_print()
{
	void* listBacktraceAddr[BACKTRACE_MAX_SIZE];
	int iBacktraceSize = backtrace (listBacktraceAddr, BACKTRACE_MAX_SIZE);
	for(int i=0; i<iBacktraceSize; i++)
	{
		void* addr = listBacktraceAddr[i];
		Dl_info info;
		dladdr(addr, &info);
		fprintf(stderr, "%lu => %s (%s)\n", addr, info.dli_fname, info.dli_sname);
	}
}