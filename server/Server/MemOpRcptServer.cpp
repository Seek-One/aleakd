//
// Created by ebeuque on 09/03/2021.
//

#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

#include "Model/MemoryOperation.h"
#include "Model/ThreadOperation.h"

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
		servermsg_version_t iProtocolVersion;
		qint64 byteRead = m_pClientSocket->read((char *) &iProtocolVersion, sizeof(iProtocolVersion));
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
		servermsg_version_t iMsgVersion;
		do {
			qint64 byteRead = m_pClientSocket->read((char *) &iMsgVersion, sizeof(iMsgVersion));
			if(byteRead > 0){
				if(iMsgVersion == 1){
					if(!doProcessMsgV1(m_pClientSocket)){
						break;
					}
				}else{
					qCritical("[aleakd-server] Unsupported message");
					m_pClientSocket->readAll();
					break;
				}
			} else {
				break;
			}
		} while (true);
	}
}

bool MemOpRcptServer::doProcessMsgV1(QAbstractSocket* pClientSocket)
{
	ServerMsgHeaderV1 header;
	qint64 byteRead = pClientSocket->read((char *) &header, sizeof(header));
	if (byteRead > 0) {

		// Memory operation
		if (header.msg_code >= 10 && header.msg_code < 20) {
			ServerMsgMemoryDataV1 data;
			qint64 byteRead = pClientSocket->read((char *) &data, sizeof(data));
			if (byteRead > 0) {
				MemoryOperation *pMemoryOperation = new MemoryOperation();
				pMemoryOperation->m_tvOperation.tv_sec = header.time_sec;
				pMemoryOperation->m_tvOperation.tv_usec = header.time_usec;
				pMemoryOperation->m_iMsgCode = (ALeakD_MsgCode) header.msg_code;
				pMemoryOperation->m_iCallerThreadId = header.thread_id;
				pMemoryOperation->m_iAllocSize = data.alloc_size;
				pMemoryOperation->m_iAllocPtr = data.alloc_ptr;
				pMemoryOperation->m_iAllocNum = data.alloc_num;
				pMemoryOperation->m_iFreePtr = data.free_ptr;

				//qDebug("[aleakd-server] msg received: type=%d, time=%lu,%lu, thread=%lu, size=%lu",
				//	msg.msg_type, msg.time_sec, msg.time_usec,
				//  msg.thread_id, msg.alloc_size);

				if (m_pHandler) {
					m_pHandler->onMemoryOperationReceived(MemoryOperationSharedPtr(pMemoryOperation));
				}
			}
		}

		// Thread operation
		if (header.msg_code >= 30 && header.msg_code < 40) {
			ServerMsgThreadDataV1 data;
			qint64 byteRead = pClientSocket->read((char *) &data, sizeof(data));
			if (byteRead > 0) {
				ThreadOperation *pThreadOperation = new ThreadOperation();
				pThreadOperation->m_tvOperation.tv_sec = header.time_sec;
				pThreadOperation->m_tvOperation.tv_usec = header.time_usec;
				pThreadOperation->m_iMsgCode = (ALeakD_MsgCode) header.msg_code;
				pThreadOperation->m_iCallerThreadId = header.thread_id;
				pThreadOperation->m_iThreadId = data.thread_id;
				pThreadOperation->m_szThreadName = data.thread_name;

				//qDebug("[aleakd-server] msg received: type=%d, time=%lu,%lu, thread=%lu, size=%lu",
				//	msg.msg_type, msg.time_sec, msg.time_usec,
				//  msg.thread_id, msg.alloc_size);

				if (m_pHandler) {
					m_pHandler->onThreadOperationReceived(ThreadOperationSharedPtr(pThreadOperation));
				}
			}
		}
	}

	return true;
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

#ifdef USE_TCP_SERVER
	m_pTcpServer = new QTcpServer();
	connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

	qCritical("[aleakd-server] Creating server on port: %d", m_iPort);
	bGoOn = m_pTcpServer->listen(QHostAddress::Any, m_iPort);
#else
	m_pServerSocket = new QUdpSocket();
	connect(m_pServerSocket, SIGNAL(connected()), this, SLOT(onNewConnection()));

	qCritical("[aleakd-server] Creating server on port: %d", m_iPort);
	bGoOn = m_pServerSocket->bind(QHostAddress::Any, m_iPort);
#endif

	exec();

#ifdef USE_TCP_SERVER
	if(m_pTcpServer){
		delete m_pTcpServer;
		m_pTcpServer = NULL;
	}
#else
	if(m_pServerSocket){
		delete m_pServerSocket;
		m_pServerSocket = NULL;
	}
#endif
}