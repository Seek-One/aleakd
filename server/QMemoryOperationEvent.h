//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_QMEMORYOPERATIONEVENT_H
#define ALEAKD_QMEMORYOPERATIONEVENT_H

#include <QEvent>

#include <QSharedPointer>

#include "MemoryOperation.h"

#define MemoryOperationEvent (QEvent::Type)(QEvent::User + 1)

class QMemoryOperationEvent : public QEvent
{
public:
	QMemoryOperationEvent(const QSharedPointer<MemoryOperation>& pMemoryOperation);
	virtual~ QMemoryOperationEvent();

public:
	QSharedPointer<MemoryOperation> m_pMemoryOperation;
};


#endif //ALEAKD_QMEMORYOPERATIONEVENT_H
