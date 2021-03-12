//
// Created by ebeuque on 12/03/2021.
//

#include "MemoryStats.h"

MemoryStats::MemoryStats()
{
	m_iOpCount = 0;
	m_iAllocSize = 0;
	m_iFreeSize = 0;
	m_iRemaingSize = 0;

	m_iMallocCount = 0;
	m_iReallocCount = 0;
	m_iCallocCount = 0;
	m_iFreeCount = 0;
	m_iPosixMemalignCount = 0;
	m_iAlignedAllocCount = 0;
	m_iMemAlignCount = 0;
	m_iVAllocCount = 0;
	m_iPVAllocCount = 0;
}

MemoryStats::~MemoryStats(){

}

void MemoryStats::reset()
{
	m_iOpCount = 0;
	m_iAllocSize = 0;
	m_iFreeSize = 0;
	m_iRemaingSize = 0;

	m_iMallocCount = 0;
	m_iReallocCount = 0;
	m_iCallocCount = 0;
	m_iFreeCount = 0;
	m_iPosixMemalignCount = 0;
	m_iAlignedAllocCount = 0;
	m_iMemAlignCount = 0;
	m_iVAllocCount = 0;
	m_iPVAllocCount = 0;
}