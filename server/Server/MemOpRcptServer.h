//
// Created by ebeuque on 09/03/2021.
//

#ifndef ALEAKD_MEMOPRCPTSERVER_H
#define ALEAKD_MEMOPRCPTSERVER_H

#include <QThread>

#include "Model/MemoryOperation.h"

#include "Server/IMemOpRcptServerHandler.h"

class QTcpServer;
class QTcpSocket;

class MemOpRcptServer : public QThread
{
	Q_OBJECT
public:
	MemOpRcptServer(QObject* parent = NULL);
	virtual ~MemOpRcptServer();

	void setPort(int iPort);

	void setHandler(IMemOpRcptServerHandler* pHandler);

protected:
	void run();

private slots:
	void onNewConnection();
	void onSocketReadyToRead();
	void onSocketDisconnected();

private:
	bool doProcessMsgV1(QTcpSocket* pClientSocket);

private:
	QTcpServer* m_pTcpServer;
	QTcpSocket* m_pClientSocket;

	int m_iState;
	short m_iProtocolVersion;

	int m_iPort;

	IMemOpRcptServerHandler* m_pHandler;
};


#endif //ALEAKD_MEMOPRCPTSERVER_H
