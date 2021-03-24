//
// Created by ebeuque on 12/03/2021.
//

#ifndef ALEAKD_IMEMOPRCPTSERVERHANDLER_H
#define ALEAKD_IMEMOPRCPTSERVERHANDLER_H

#include "Model/MemoryOperation.h"
#include "Model/ThreadOperation.h"
#include "Model/Backtrace.h"

class IMemOpRcptServerHandler
{
public:
	IMemOpRcptServerHandler();
	virtual ~IMemOpRcptServerHandler();

public:
	virtual void onNewConnection();
	virtual void onMemoryOperationReceived(const MemoryOperationSharedPtr& pMemoryOperation);
	virtual void onThreadOperationReceived(const ThreadOperationSharedPtr& pThreadOperation);
	virtual void onBacktraceReceived(const BacktraceSharedPtr& pBackTrace);
};

#endif //ALEAKD_IMEMOPRCPTSERVERHANDLER_H
