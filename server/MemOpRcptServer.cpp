//
// Created by ebeuque on 09/03/2021.
//

#include <QTcpSocket>

#include "../shared/server-msg.h"

#include "MemOpRcptServer.h"

MemOpRcptServer::MemOpRcptServer(QObject* parent)
	: QTcpServer(parent)
{
	m_pClientSocket = NULL;
	m_iState = 0;
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
	qInfo("[aleakd-server] New connection on socket %lu", socketDescriptor);
	QTcpSocket* pSocket = new QTcpSocket();
	if(pSocket) {
		pSocket->setSocketDescriptor(socketDescriptor);
	}
	if(!m_pClientSocket) {
		connect(pSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyToRead()));
		connect(pSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));

		m_pClientSocket = pSocket;
	}else{
		qInfo("[aleakd-server] A connection is already present, closing the connection");
		if(pSocket) {
			pSocket->close();
			delete pSocket;
			pSocket = NULL;
		}
	}
}

void MemOpRcptServer::onSocketReadyToRead()
{
	// Read protocol version
	if(m_iState == 0)
	{
		uint8_t iProtocolVersion;
		qint64 byteRead = m_pClientSocket->read((char *) &iProtocolVersion, sizeof(uint8_t));
		if(byteRead > 0){
			m_iState = 1;
			m_iProtocolVersion = iProtocolVersion;
			qDebug("[aleakd-server] Client using protocol version %d", m_iProtocolVersion);
		}else{
			qDebug("[aleakd-server] Unable to get version, closing connection");
			m_pClientSocket->close();
		}
	}

	// Read message
	if(m_iState == 1)
	{
		ServerMemoryMsgV1 msg;
		do {
			qint64 byteRead = m_pClientSocket->read((char *) &msg, sizeof(msg));
			if (byteRead > 0) {
				qDebug("[aleakd-server] msg: %d time=%lu,%lu", msg.msg_type, msg.time_sec, msg.time_usec);
			} else {
				break;
			}
		} while (true);
	}
}

void MemOpRcptServer::onSocketDisconnected()
{
	if(m_pClientSocket){
		qInfo("[aleakd-server] Closing the connection");
		m_pClientSocket->close();
		m_pClientSocket = NULL;
	}
}