//
// Created by ebeuque on 10/03/2021.
//

#include "MemoryOperation.h"

MemoryOperation::MemoryOperation()
{
	m_tvOperation.tv_sec = 0;
	m_tvOperation.tv_usec = 0;
	m_iMsgCode = ALeakD_MsgCode_unknown;

	// Current thread
	m_iCallerThreadId = 0;

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

bool MemoryOperation::hasAllocOperation() const
{
	switch (m_iMsgCode) {
	case ALeakD_MsgCode_malloc:
	case ALeakD_MsgCode_calloc:
	case ALeakD_MsgCode_realloc:
	case ALeakD_MsgCode_free:
	case ALeakD_MsgCode_posix_memalign:
	case ALeakD_MsgCode_aligned_alloc:
	case ALeakD_MsgCode_memalign:
	case ALeakD_MsgCode_valloc:
	case ALeakD_MsgCode_pvalloc:
		return true;
	default:
		break;
	}
	return false;
}

bool MemoryOperation::hasFreeOperation() const
{
	switch (m_iMsgCode) {
	case ALeakD_MsgCode_realloc:
	case ALeakD_MsgCode_free:
		return true;
	default:
		break;
	}
	return false;
}

MemoryOperationList::MemoryOperationList()
{

}

MemoryOperationList::~MemoryOperationList()
{

}

MemoryOperationSharedPtr MemoryOperationList::getPtrNotFreed(uint64_t iPtrAddr) const
{
	MemoryOperationList::const_iterator iter;
	iter = constBegin();
	while(iter != constEnd())
	{
		const MemoryOperationSharedPtr& pMemoryOperation = (*iter);
		if(pMemoryOperation->m_iAllocPtr == iPtrAddr && !pMemoryOperation->m_bFreed)
		{
			return pMemoryOperation;
		}
		iter++;
	}

	return MemoryOperationSharedPtr();
}