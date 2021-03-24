//
// Created by ebeuque on 17/03/2021.
//

#include <sys/time.h>

#include "ThreadOperation.h"

ThreadOperation::ThreadOperation()
{
	timerclear(&m_tvOperation);
	m_iMsgNum = 0;
	m_iMsgCode = ALeakD_MsgCode_unknown;
	m_iCallerThreadId = 0;
	m_iThreadId = 0;
}

ThreadOperation::~ThreadOperation()
{

}