//
// Created by ebeuque on 09/03/2021.
//

#include <QTcpSocket>

#include "MemOpRcptServer.h"

MemOpRcptServer::MemOpRcptServer(QObject* parent)
	: QTcpServer(parent)
{
	m_pClientSocket = NULL;
}

MemOpRcptServer::~MemOpRcptServer()
{
	if(m_pClientSocket){
		delete m_pClientSocket;
		m_pClientSocket;
	}
}

void MemOpRcptServer::incomingConnection(qintptr socketDescriptor)
{
	qInfo("[aleak-server] New connection on socket %lu", socketDescriptor);
	QTcpSocket* pSocket = new QTcpSocket();
	if(pSocket) {
		pSocket->setSocketDescriptor(socketDescriptor);
	}
	if(m_pClientSocket) {
		m_pClientSocket = pSocket;
	}else{
		if(pSocket) {
			pSocket->close();
			delete pSocket;
			pSocket = NULL;
		}
	}
}