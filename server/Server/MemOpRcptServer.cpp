//
// Created by ebeuque on 09/03/2021.
//

#include <QTcpSocket>
#include <QTcpServer>

#include "../shared/server-msg.h"

#include "MemOpRcptServer.h"

MemOpRcptServer::MemOpRcptServer(QObject* parent)
	: QThread(parent)
{
	m_pClientSocket = NULL;
	m_iState = 0;
	m_pHandler = NULL;

	m_iPort = 0;

	moveToThread(this);
}

MemOpRcptServer::~MemOpRcptServer()
{
	if(m_pClientSocket){
		delete m_pClientSocket;
		m_pClientSocket;
	}
}

void MemOpRcptServer::setPort(int iPort)
{
	m_iPort = iPort;
}

void MemOpRcptServer::setHandler(IMemOpRcptServerHandler* pHandler)
{
	m_pHandler = pHandler;
}

void MemOpRcptServer::onNewConnection()
{
	QTcpSocket* pSocket;

	if(m_pTcpServer->hasPendingConnections()){
		pSocket = m_pTcpServer->nextPendingConnection();
	}

	if(!pSocket){
		return;
	}

	qInfo("[aleakd-server] New connection on socket %lu", pSocket->socketDescriptor());
	if(!m_pClientSocket) {
		connect(pSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyToRead()));
		connect(pSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));

		m_pClientSocket = pSocket;
		m_iState = 0;
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

			if(m_pHandler){
				m_pHandler->onNewConnection();
			}
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
				MemoryOperation* pMemoryOperation = new MemoryOperation();

				pMemoryOperation->m_tvOperation.tv_sec = msg.time_sec;
				pMemoryOperation->m_tvOperation.tv_usec = msg.time_usec;
				pMemoryOperation->m_iMemOpType = (ALeakD_AllocType)msg.msg_type;
				pMemoryOperation->m_iThreadId = msg.thread_id;
				pMemoryOperation->m_iAllocSize = msg.alloc_size;
				pMemoryOperation->m_iAllocPtr = msg.alloc_ptr;
				pMemoryOperation->m_iAllocNum = msg.alloc_num;
				pMemoryOperation->m_iFreePtr = msg.free_ptr;

				//qDebug("[aleakd-server] msg received: type=%d, time=%lu,%lu, thread=%lu, size=%lu",
		   		//	msg.msg_type, msg.time_sec, msg.time_usec,
				//  msg.thread_id, msg.alloc_size);

				if(m_pHandler){
					m_pHandler->onMemoryOperationReceived(MemoryOperationSharedPtr(pMemoryOperation));
				}

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

void MemOpRcptServer::run()
{
	bool bGoOn;

	m_pTcpServer = new QTcpServer();
	connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

	qCritical("[aleakd-server] Creating server on port: %d", m_iPort);
	bGoOn = m_pTcpServer->listen(QHostAddress::Any, m_iPort);

	exec();

	if(m_pTcpServer){
		delete m_pTcpServer;
		m_pTcpServer = NULL;
	}
}