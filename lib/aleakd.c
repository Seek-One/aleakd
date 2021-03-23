/*
 * mymalloc.c - Examples of run-time, link-time, and compile-time
 *              library interposition.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "wrapper.h"

#include "aleakd.h"

static unsigned long g_iAllocNumber = 0;

void aleakd_data_incr_alloc_number()
{
	g_iAllocNumber++;
}

unsigned long aleakd_data_get_alloc_number()
{
	return g_iAllocNumber;
}

void __attribute__((constructor)) aleakd_constructor()
{
	fprintf(stderr, "[aleakd] init\n");
	if(wrapper_init() != 0){
		fprintf(stderr, "[aleakd] failed to init\n");
		exit(-1);
	}
}

void __attribute__((destructor)) aleakd_destructor()
{
	fprintf(stderr, "[aleakd] dispose\n");
	wrapper_dispose();
	sleep(2);
}
