//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_GLOBAL_CONST_H
#define ALEAKD_GLOBAL_CONST_H

#define ALEAKD_MSG_VERSION 1

enum ALeakD_MsgCode {
	ALeakD_MsgCode_unknown = 0,
	ALeakD_MsgCode_init = 1,
	ALeakD_MsgCode_dispose = 2,

	// memory function
	ALeakD_MsgCode_malloc = 11,
	ALeakD_MsgCode_free = 12,
	ALeakD_MsgCode_calloc = 13,
	ALeakD_MsgCode_realloc = 14,
	ALeakD_MsgCode_posix_memalign = 15,
	ALeakD_MsgCode_aligned_alloc = 16,
	ALeakD_MsgCode_memalign = 17,
	ALeakD_MsgCode_valloc = 18,
	ALeakD_MsgCode_pvalloc = 19,

	// pthread function
	ALeakD_MsgCode_pthread_create = 31,
	ALeakD_MsgCode_pthread_set_name = 32,
};

const char* ALeakD_MsgLabel(enum ALeakD_MsgCode iFuncType);

void ALeakD_toHexString(const unsigned char* bytes, size_t size, char* szString, int iStrSize);

#endif //ALEAKD_GLOBAL_CONST_H
