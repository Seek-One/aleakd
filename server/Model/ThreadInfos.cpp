//
// Created by ebeuque on 17/03/2021.
//

#include "ThreadInfos.h"

ThreadInfos::ThreadInfos()
{
	m_iThreadId = 0;

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