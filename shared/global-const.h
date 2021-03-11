//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_GLOBAL_CONST_H
#define ALEAKD_GLOBAL_CONST_H

#define ALEAKD_PROTOCOL_VERSION 1
#define ALEAKD_MSG_VERSION 1

enum ALeakD_AllocType {
	ALeakD_malloc = 1,
	ALeakD_free = 2,
	ALeakD_calloc = 3,
	ALeakD_realloc = 4,
	ALeakD_posix_memalign = 5,
	ALeakD_aligned_alloc = 6,
	ALeakD_memalign = 7,
	ALeakD_valloc = 8,
	ALeakD_pvalloc = 9
};

const char* ALeakD_TypeName(enum ALeakD_AllocType iType);

#endif //ALEAKD_GLOBAL_CONST_H
