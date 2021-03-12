//
// Created by ebeuque on 09/03/2021.
//

#ifndef ALEAKD_MEMOPRCPTSERVER_H
#define ALEAKD_MEMOPRCPTSERVER_H

#include <QTcpServer>

#include "MemoryOperation.h"
#include "IMemOpRcptServerHandler.h"

class QTcpSocket;

class MemOpRcptServer : public QTcpServer
{
	Q_OBJECT
public:
	MemOpRcptServer(QObject* parent = NULL);
	virtual ~MemOpRcptServer();

	void setHandler(IMemOpRcptServerHandler* pHandler);

protected:
	void incomingConnection(qintptr socketDescriptor);

private slots:
	void onSocketReadyToRead();
	void onSocketDisconnected();

private:
	QTcpSocket* m_pClientSocket;

	int m_iState;
	uint8_t m_iProtocolVersion;

	IMemOpRcptServerHandler* m_pHandler;
};


#endif //ALEAKD_MEMOPRCPTSERVER_H
