//
// Created by ebeuque on 12/03/2021.
//

#ifndef ALEAKD_MEMORYSTATS_H
#define ALEAKD_MEMORYSTATS_H

#include <stdint.h>

class MemoryStats
{
public:
	MemoryStats();
	virtual ~MemoryStats();

	void reset();

public:
	int m_iMessageCount;

	int m_iMemoryOperationCount;
	uint64_t m_iMemoryOperationSize;

	int m_iTotalAllocCount;
	uint64_t m_iTotalAllocSize;

	int m_iTotalFreeCount;
	uint64_t m_iTotalFreeSize;

	int m_iTotalRemainingCount;
	uint64_t m_iTotalRemainingSize;

	int m_iMallocCount;
	int m_iReallocCount;
	int m_iCallocCount;
	int m_iPosixMemalignCount;
	int m_iAlignedAllocCount;
	int m_iMemAlignCount;
	int m_iVAllocCount;
	int m_iPVAllocCount;
	int m_iFreeCount;
};


#endif //ALEAKD_MEMORYSTATS_H
