//
// Created by ebeuque on 10/03/2021.
//

#include "QMemoryOperationEvent.h"

QMemoryOperationEvent::QMemoryOperationEvent(const QSharedPointer<MemoryOperation>& pMemoryOperation)
	: QEvent(MemoryOperationEvent)
{
	m_pMemoryOperation = pMemoryOperation;
}

QMemoryOperationEvent::~QMemoryOperationEvent()
{

}