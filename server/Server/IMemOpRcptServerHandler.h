//
// Created by ebeuque on 12/03/2021.
//

#ifndef ALEAKD_IMEMOPRCPTSERVERHANDLER_H
#define ALEAKD_IMEMOPRCPTSERVERHANDLER_H

#include "Model/MemoryOperation.h"
#include "Model/ThreadOperation.h"
#include "Model/Backtrace.h"
#include "Model/SymbolInfos.h"

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
	virtual void onSymbolInfosReceived(const SymbolInfosSharedPtr& pSymbolInfos);
};

#endif //ALEAKD_IMEMOPRCPTSERVERHANDLER_H
