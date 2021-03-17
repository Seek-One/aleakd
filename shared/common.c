//
// Created by ebeuque on 11/03/2021.
//

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