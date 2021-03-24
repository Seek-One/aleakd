//
// Created by ebeuque on 24/03/2021.
//

#ifndef ALEAKD_BACKTRACE_H
#define ALEAKD_BACKTRACE_H

#include <QSharedPointer>

class Backtrace;
typedef QSharedPointer<Backtrace> BacktraceSharedPtr;

class Backtrace {
public:
	Backtrace();
	virtual ~Backtrace();

public:
	int m_iOriginMsgNum;

	QList<quint64> m_listAddr;
};


#endif //ALEAKD_BACKTRACE_H
