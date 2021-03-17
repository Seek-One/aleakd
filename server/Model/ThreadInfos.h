//
// Created by ebeuque on 17/03/2021.
//

#ifndef ALEAKD_THREADINFOS_H
#define ALEAKD_THREADINFOS_H

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

	uint64_t m_iCurrentSize;
	uint64_t m_iPeakSize;
};

class ThreadInfosList : public QList<ThreadInfosSharedPtr>
{
public:
	ThreadInfosList();
	virtual ~ThreadInfosList();

	ThreadInfosSharedPtr getById(uint64_t iThreadId) const;
};


#endif //ALEAKD_THREADINFOS_H
