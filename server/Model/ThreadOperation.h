//
// Created by ebeuque on 17/03/2021.
//

#ifndef ALEAKD_THREADOPERATION_H
#define ALEAKD_THREADOPERATION_H

#include <QSharedPointer>

extern "C" {
#include "../shared/global-const.h"
};

class ThreadOperation;
typedef QSharedPointer<ThreadOperation> ThreadOperationSharedPtr;

class ThreadOperation
{
public:
	ThreadOperation();
	virtual ~ThreadOperation();

public:
	struct timeval m_tvOperation;
	uint32_t m_iMsgNum;
	// Msg type
	ALeakD_MsgCode m_iMsgCode;
	// Thread
	uint64_t m_iCallerThreadId;

	// Operation data
	uint64_t m_iThreadId;
	QString m_szThreadName;
};


#endif //ALEAKD_THREADOPERATION_H
