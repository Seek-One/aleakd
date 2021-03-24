//
// Created by ebeuque on 24/03/2021.
//

#ifndef ALEAKD_SYMBOLINFOS_H
#define ALEAKD_SYMBOLINFOS_H

#include <QSharedPointer>

class SymbolInfos;
typedef QSharedPointer<SymbolInfos> SymbolInfosSharedPtr;

class SymbolInfos {
public:
	SymbolInfos();
	virtual ~SymbolInfos();

public:
	quint64 m_iAddr;
	QString m_szObjectName;
	quint64 m_iObjectAddr;
	QString m_szSymbolName;
	quint64 m_iSymbolAddr;
};


class SymbolInfosList : public QHash<quint64, SymbolInfosSharedPtr>
{
public:
	SymbolInfosList();
	virtual ~SymbolInfosList();

	SymbolInfosSharedPtr getByAddr(uint64_t iAddr) const;
};

#endif //ALEAKD_SYMBOLINFOS_H
