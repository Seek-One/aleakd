//
// Created by ebeuque on 17/03/2021.
//

#include "ThreadInfos.h"

ThreadInfos::ThreadInfos()
{
	m_iThreadId = 0;

	m_iCreationMsgNum = 0;
	m_iLastAllocMsgNum = 0;

	timerclear(&m_tvCreation);
	timerclear(&m_tvTermination);
	m_bIsTerminated = false;

	m_iCurrentAllocCount = 0;
	m_iCurrentSize = 0;
	m_iTotalAllocCount = 0;
	m_iPeakSize = 0;
}

ThreadInfos::~ThreadInfos()
{

}

ThreadInfosList::ThreadInfosList()
{

}

ThreadInfosList::~ThreadInfosList()
{

}

ThreadInfosSharedPtr ThreadInfosList::getById(uint64_t iThreadId) const
{
	ThreadInfosList::const_iterator iter;
	iter = constBegin();
	while(iter != constEnd())
	{
		const ThreadInfosSharedPtr& pThreadInfos = (*iter);
		if(pThreadInfos->m_iThreadId == iThreadId)
		{
			return pThreadInfos;
		}
		iter++;
	}
	return ThreadInfosSharedPtr();
}

ThreadInfosSharedPtr ThreadInfosList::getByIdAndMsgNum(uint64_t iThreadId, uint32_t iMsgNum) const
{
	ThreadInfosList::const_iterator iter;
	iter = constBegin();
	while(iter != constEnd())
	{
		const ThreadInfosSharedPtr& pThreadInfos = (*iter);
		if(pThreadInfos->m_iThreadId == iThreadId)
		{
			if(pThreadInfos->m_iCreationMsgNum <= iMsgNum && iMsgNum <= pThreadInfos->m_iLastAllocMsgNum)
			{
				return pThreadInfos;
			}
		}
		iter++;
	}
	return ThreadInfosSharedPtr();
}

void ThreadInfosList::removeById(uint64_t iThreadId)
{
	ThreadInfosList::iterator iter;
	iter = begin();
	while(iter != end())
	{
		const ThreadInfosSharedPtr& pThreadInfos = (*iter);
		if(pThreadInfos->m_iThreadId == iThreadId)
		{
			iter = erase(iter);
		}else {
			iter++;
		}
	}
}