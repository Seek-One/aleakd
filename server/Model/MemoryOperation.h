//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_MEMORYOPERATION_H
#define ALEAKD_MEMORYOPERATION_H

#include <stdint.h>
#include <time.h>

#include <QSharedPointer>
#include <QList>

extern "C" {
#include "../shared/global-const.h"
};

class MemoryOperation;
typedef QSharedPointer<MemoryOperation> MemoryOperationSharedPtr;

class MemoryOperation
{
public:
	MemoryOperation();
	virtual ~MemoryOperation();

	bool hasAllocOperation() const;
	bool hasFreeOperation() const;

public:
	struct timeval m_tvOperation;
	// Msg type
	ALeakD_MsgCode m_iMsgCode;
	// Current thread
	uint64_t m_iCallerThreadId;

	// Alloc infos
	uint64_t m_iAllocSize;
	uint64_t m_iAllocPtr;
	uint32_t m_iAllocNum;

	// Free infos
	uint64_t m_iFreePtr;
	bool m_bFreed;
};

class MemoryOperationList : public QList<MemoryOperationSharedPtr>
{
public:
	MemoryOperationList();
	virtual ~MemoryOperationList();

	MemoryOperationSharedPtr getPtrNotFreed(uint64_t iPtrAddr) const;
};

#endif //ALEAKD_MEMORYOPERATION_H
