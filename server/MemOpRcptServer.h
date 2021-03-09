//
// Created by ebeuque on 09/03/2021.
//

#ifndef ALEAKD_MEMOPRCPTSERVER_H
#define ALEAKD_MEMOPRCPTSERVER_H

#include <QTcpServer>

class QTcpSocket;

class MemOpRcptServer : public QTcpServer
{
public:
	MemOpRcptServer(QObject* parent = NULL);
	virtual ~MemOpRcptServer();

protected:
	void incomingConnection(qintptr socketDescriptor);

private:
	QTcpSocket* m_pClientSocket;
};


#endif //ALEAKD_MEMOPRCPTSERVER_H
