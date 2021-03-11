//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_MEMORYOPERATION_H
#define ALEAKD_MEMORYOPERATION_H

#include <stdint.h>
#include <time.h>

#include "../shared/global-const.h"

class MemoryOperation
{
public:
	MemoryOperation();
	virtual ~MemoryOperation();

public:
	struct timeval m_tvOperation;

	// Msg type
	ALeakD_AllocType m_iMemOpType; // ALeakD_AllocType

	// Current thread
	uint64_t m_iThreadId;

	// Alloc infos
	uint64_t m_iAllocSize;
	uint64_t m_iAllocPtr;
	uint32_t m_iAllocNum;

	// Free infos
	uint64_t m_iFreePtr;
};


#endif //ALEAKD_MEMORYOPERATION_H