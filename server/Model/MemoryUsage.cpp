//
// Created by ebeuque on 30/03/2021.
//

#include "MemoryUsage.h"

MemoryUsage::MemoryUsage()
{
	m_iFirstTime = 0;
	m_iLastTime = 0;
}

MemoryUsage::~MemoryUsage()
{

}

void MemoryUsage::clear()
{
	m_iFirstTime = 0;
	m_iLastTime = 0;
	m_listSizeByTimestamp.clear();
}

bool MemoryUsage::contains(quint64 iTimestamp) const
{
	return m_listSizeByTimestamp.contains(iTimestamp);
}

int  MemoryUsage::count() const
{
	return m_listSizeByTimestamp.count();
}

void MemoryUsage::update(quint64 iTimestamp, quint64 iSize)
{
	int iCurrentSize = m_listSizeByTimestamp.value(iTimestamp, 0);
	if(iCurrentSize == 0){
		if(m_iFirstTime == 0){
			m_iFirstTime = iTimestamp;
		}
		m_listSizeByTimestamp.insert(iTimestamp, iSize);
		m_iLastTime = iTimestamp;
	}else{
		if(iCurrentSize < iSize){
			m_listSizeByTimestamp.insert(iTimestamp, iSize);
			m_iLastTime = iTimestamp;
		}
	}
}

quint64 MemoryUsage::value(quint64 iTimestamp)
{
	return m_listSizeByTimestamp.value(iTimestamp, 0);
}