//
// Created by ebeuque on 11/03/2021.
//

#include <stdlib.h>

#include "global-const.h"

const char* ALeakD_MsgLabel(enum ALeakD_MsgCode iMsgCode)
{
	switch (iMsgCode) {
	// memory allocation
	case ALeakD_MsgCode_malloc:
		return "malloc";
	case ALeakD_MsgCode_free:
		return "free";
	case ALeakD_MsgCode_calloc:
		return "calloc";
	case ALeakD_MsgCode_realloc:
		return "realloc";
	case ALeakD_MsgCode_posix_memalign:
		return "posix_memalign";
	case ALeakD_MsgCode_aligned_alloc:
		return "aligned_alloc";
	case ALeakD_MsgCode_memalign:
		return "memalign";
	case ALeakD_MsgCode_valloc:
		return "valloc";
	case ALeakD_MsgCode_pvalloc:
		return "pvalloc";
	// pthread
	case ALeakD_MsgCode_pthread_create:
		return "pthread_create";
	case ALeakD_MsgCode_pthread_set_name:
		return "pthread_set_name";
	}
	return "";
}

static char g_szValBase16[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

void ALeakD_toHexString(const unsigned char* bytes, size_t size, char* szString, int iStrSize)
{
	if(size > 0 && bytes != NULL){
		for(size_t j = 0; j < size; j++){
			szString[j*2] = g_szValBase16[((bytes[j] >> 4) & 0xF)];
			szString[(j*2) + 1] = g_szValBase16[(bytes[j]) & 0x0F];
		}
		szString[size*2] = '\0';
	}
}