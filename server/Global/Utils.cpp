//
// Created by ebeuque on 17/03/2021.
//

#include "Utils.h"

QString Utils::getBeautifulNumberString(const QString& szNumber)
{
	int iLen = szNumber.count();
	int iPad = iLen % 3;

	QString szNewString;

	for(int i=0; i<iLen; i++)
	{
		if(i != 0 && (i - iPad) % 3 == 0)
		{
			szNewString += " ";
		}
		szNewString.append(szNumber.at(i));
	}

	return szNewString;
}