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
	int m_iOpCount;
	uint64_t m_iAllocSize;
	uint64_t m_iFreeSize;
	uint64_t m_iRemaingSize;

	int m_iMallocCount;
	int m_iReallocCount;
	int m_iCallocCount;
	int m_iFreeCount;
	int m_iPosixMemalignCount;
	int m_iAlignedAllocCount;
	int m_iMemAlignCount;
	int m_iVAllocCount;
	int m_iPVAllocCount;
};


#endif //ALEAKD_MEMORYSTATS_H
