//
// Created by ebeuque on 12/03/2021.
//

#ifndef ALEAKD_IMEMOPRCPTSERVERHANDLER_H
#define ALEAKD_IMEMOPRCPTSERVERHANDLER_H

#include "Model/MemoryOperation.h"
#include "Model/ThreadOperation.h"

class IMemOpRcptServerHandler
{
public:
	IMemOpRcptServerHandler();
	virtual ~IMemOpRcptServerHandler();

	virtual void onNewConnection();
	virtual void onMemoryOperationReceived(const MemoryOperationSharedPtr& pMemoryOperation);
	virtual void onThreadOperationReceived(const ThreadOperationSharedPtr& pThreadOperation);
};

#endif //ALEAKD_IMEMOPRCPTSERVERHANDLER_H
