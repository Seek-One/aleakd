//
// Created by ebeuque on 10/03/2021.
//

#include "MemoryOperation.h"

MemoryOperation::MemoryOperation()
{
	m_tvOperation.tv_sec = 0;
	m_tvOperation.tv_usec = 0;
	m_iMemOpType = ALeakD_unknown;

	// Current thread
	m_iThreadId = 0;

	// Alloc infos
	m_iAllocSize = 0;
	m_iAllocPtr = 0;
	m_iAllocNum = 0;

	// Free infos
	m_iFreePtr = 0;
	m_bFreed = false;
}

MemoryOperation::~MemoryOperation()
{

}

MemoryOperationList::MemoryOperationList()
{

}

MemoryOperationList::~MemoryOperationList()
{

}

MemoryOperationSharedPtr MemoryOperationList::getPtrNotFreed(uint64_t iFreePtr) const
{
	MemoryOperationList::const_iterator iter;
	iter = constBegin();
	while(iter != constEnd())
	{
		const MemoryOperationSharedPtr& pMemoryOperation = (*iter);
		if(pMemoryOperation->m_iFreePtr == iFreePtr && !pMemoryOperation->m_bFreed)
		{
			return pMemoryOperation;
		}
		iter++;
	}

	return MemoryOperationSharedPtr();
}