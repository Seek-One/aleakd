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
class QUdpSocket;
class QIODevice;

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
	// TCP mode
	void onNewConnection();
	void onSocketReadyToRead();
	void onSocketDisconnected();
	// UDP mode
	void onPendingDatagramToRead();

private:
	bool doReadMsg(QIODevice* pIODevice);
	bool doProcessMsgV1(QIODevice* pIODevice);

private:
	int m_iPort;
	bool m_bUseTCP;

	// TCP mode
	QTcpServer* m_pTcpServer;
	QTcpSocket* m_pTcpClientSocket;

	// UDP mode
	QUdpSocket* m_pUdpServerSocket;

	IMemOpRcptServerHandler* m_pHandler;
};


#endif //ALEAKD_MEMOPRCPTSERVER_H
