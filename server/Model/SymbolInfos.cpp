//
// Created by ebeuque on 24/03/2021.
//

#include "SymbolInfos.h"

SymbolInfos::SymbolInfos()
{
	m_iAddr = 0;
	m_iObjectAddr = 0;
	m_iSymbolAddr = 0;
}

SymbolInfos::~SymbolInfos()
{

}

SymbolInfosList::SymbolInfosList()
{

}

SymbolInfosList::~SymbolInfosList()
{

}

SymbolInfosSharedPtr SymbolInfosList::getByAddr(uint64_t iAddr) const
{
	if(contains(iAddr)){
		return value(iAddr);
	}
	return SymbolInfosSharedPtr();
}