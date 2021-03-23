//
// Created by ebeuque on 17/03/2021.
//

#ifndef ALEAKD_THREADINFOS_H
#define ALEAKD_THREADINFOS_H

#include <sys/time.h>

#include <QList>
#include <QSharedPointer>

class ThreadInfos;
typedef QSharedPointer<ThreadInfos> ThreadInfosSharedPtr;

class ThreadInfos
{
public:
	ThreadInfos();
	virtual ~ThreadInfos();

public:
	uint64_t m_iThreadId;
	QString m_szThreadName;

	struct timeval m_tvCreation;
	struct timeval m_tvTermination;
	bool m_bIsTerminated;

	uint32_t  m_iCurrentAllocCount;
	uint64_t m_iCurrentSize;
	uint32_t  m_iTotalAllocCount;
	uint64_t m_iPeakSize;
};

class ThreadInfosList : public QList<ThreadInfosSharedPtr>
{
public:
	ThreadInfosList();
	virtual ~ThreadInfosList();

	ThreadInfosSharedPtr getById(uint64_t iThreadId) const;
	void removeById(uint64_t iThreadId);
};


#endif //ALEAKD_THREADINFOS_H
