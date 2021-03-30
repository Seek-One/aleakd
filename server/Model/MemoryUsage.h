//
// Created by ebeuque on 30/03/2021.
//

#ifndef ALEAKD_MEMORYUSAGE_H
#define ALEAKD_MEMORYUSAGE_H

#include <QHash>

typedef QHash<quint64, quint64> MemoryUsageEntryList;

class MemoryUsage
{
public:
	MemoryUsage();
	virtual ~MemoryUsage();

	void clear();

	bool contains(quint64 iTimestamp) const;
	int count() const;
	void update(quint64 iTimestamp, quint64 iSize);
	quint64 value(quint64 iTimestamp);

public:
	quint64 m_iFirstTime;
	quint64 m_iLastTime;
	MemoryUsageEntryList m_listSizeByTimestamp;
};


#endif //ALEAKD_MEMORYUSAGE_H
