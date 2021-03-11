//
// Created by ebeuque on 11/03/2021.
//

#include "global-const.h"

const char* ALeakD_TypeName(enum ALeakD_AllocType iType)
{
	switch (iType) {
	case ALeakD_malloc:
		return "malloc";
	case ALeakD_free:
		return "free";
	case ALeakD_calloc:
		return "calloc";
	case ALeakD_realloc:
		return "realloc";
	case ALeakD_posix_memalign:
		return "posix_memalign";
	case ALeakD_aligned_alloc:
		return "aligned_alloc";
	case ALeakD_memalign:
		return "memalign";
	case ALeakD_valloc:
		return "valloc";
	case ALeakD_pvalloc:
		return "pvalloc";
	}
	return "";
}