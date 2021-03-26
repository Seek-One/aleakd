//
// Created by ebeuque on 12/03/2021.
//

#include "MemoryStats.h"

MemoryStats::MemoryStats()
{
	m_iMessageCount = 0;

	timerclear(&m_tvStartTime);
	timerclear(&m_tvLastTime);

	m_iMemoryOperationCount = 0;
	m_iMemoryOperationSize = 0;

	m_iTotalAllocCount = 0;
	m_iTotalAllocSize = 0;

	m_iTotalFreeCount = 0;
	m_iTotalFreeSize = 0;

	m_iTotalRemainingCount = 0;
	m_iTotalRemainingSize = 0;

	m_iMallocCount = 0;
	m_iReallocCount = 0;
	m_iCallocCount = 0;
	m_iPosixMemalignCount = 0;
	m_iAlignedAllocCount = 0;
	m_iMemAlignCount = 0;
	m_iVAllocCount = 0;
	m_iPVAllocCount = 0;
	m_iFreeCount = 0;
}

MemoryStats::~MemoryStats(){

}

void MemoryStats::reset()
{
	m_iMessageCount = 0;

	timerclear(&m_tvStartTime);
	timerclear(&m_tvLastTime);

	m_iMemoryOperationCount = 0;
	m_iMemoryOperationSize = 0;

	m_iTotalAllocCount = 0;
	m_iTotalAllocSize = 0;

	m_iTotalFreeCount = 0;
	m_iTotalFreeSize = 0;

	m_iTotalRemainingCount = 0;
	m_iTotalRemainingSize = 0;

	m_iMallocCount = 0;
	m_iReallocCount = 0;
	m_iCallocCount = 0;
	m_iPosixMemalignCount = 0;
	m_iAlignedAllocCount = 0;
	m_iMemAlignCount = 0;
	m_iVAllocCount = 0;
	m_iPVAllocCount = 0;
	m_iFreeCount = 0;
}