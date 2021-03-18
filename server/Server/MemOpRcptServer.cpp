//
// Created by ebeuque on 09/03/2021.
//

#include "../../config.h"

#include <sys/time.h>

#include <QDateTime>
#include <QBuffer>

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
	m_iPort = 0;
	m_bUseTCP = false;

#ifdef USE_TCP_SERVER
	m_bUseTCP = true;
#endif

	m_pTcpClientSocket = NULL;
	m_pUdpServerSocket = NULL;

	m_pHandler = NULL;

	moveToThread(this);
}

MemOpRcptServer::~MemOpRcptServer()
{
	if(m_pTcpClientSocket){
		delete m_pTcpClientSocket;
		m_pTcpClientSocket;
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
	if(!m_pTcpClientSocket) {
		connect(pSocket, SIGNAL(readyRead()), this, SLOT(onSocketReadyToRead()));
		connect(pSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));

		m_pTcpClientSocket = pSocket;
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
	// Read message
	do {
		if(!doReadMsg(m_pTcpClientSocket)){
			break;
		}
	} while (true);
}

bool MemOpRcptServer::doReadMsg(QIODevice* pIODevice)
{
	bool bRes = false;

	// Read message version to decide how to process the message
	servermsg_version_t iMsgVersion;
	qint64 byteRead = pIODevice->read((char *) &iMsgVersion, sizeof(iMsgVersion));
	if(byteRead > 0){
		if(iMsgVersion == 1){
			bRes = doProcessMsgV1(pIODevice);
			if(!bRes){
				qCritical("[aleakd-server] Unsupported message");
			}
		}else{
			qCritical("[aleakd-server] Unsupported message");
			pIODevice->readAll();
		}
	}

	return bRes;
}

bool MemOpRcptServer::doProcessMsgV1(QIODevice* pIODevice)
{
	ServerMsgHeaderV1 header;
	qint64 byteRead = pIODevice->read((char *) &header, sizeof(header));
	if (byteRead > 0) {

		// Application message
		if (header.msg_code >= 0 && header.msg_code < 10)
		{
			if(header.msg_code == ALeakD_MsgCode_init)
			{
				if (m_pHandler) {
					m_pHandler->onNewConnection();
				}
			}
		}

		// Memory operation
		if (header.msg_code >= 10 && header.msg_code < 20)
		{
			ServerMsgMemoryDataV1 data;
			qint64 byteRead = pIODevice->read((char *) &data, sizeof(data));
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

//				struct timeval tvNow;
//				struct timeval tvRecpt;
//				gettimeofday(&tvNow, NULL);
//				timersub(&tvNow, &pMemoryOperation->m_tvOperation, &tvRecpt);

				if (m_pHandler) {
					m_pHandler->onMemoryOperationReceived(MemoryOperationSharedPtr(pMemoryOperation));
				}

//				struct timeval tvProcess;
//				gettimeofday(&tvNow, NULL);
//				timersub(&tvNow, &pMemoryOperation->m_tvOperation, &tvProcess);
				//qDebug("[aleakd-latency] latency=%lu,%06lu, process=%lu,%06lu", tvRecpt.tv_sec, tvRecpt.tv_usec, tvProcess.tv_sec, tvProcess.tv_usec);
			}
		}

		// Thread operation
		if (header.msg_code >= 30 && header.msg_code < 40) {
			ServerMsgThreadDataV1 data;
			qint64 byteRead = pIODevice->read((char *) &data, sizeof(data));
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
	if(m_pTcpClientSocket){
		qInfo("[aleakd-server] Closing the connection");
		m_pTcpClientSocket->close();
		m_pTcpClientSocket = NULL;
	}
}


void MemOpRcptServer::onPendingDatagramToRead()
{
	while (m_pUdpServerSocket->hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(m_pUdpServerSocket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		qint64 iReadData = m_pUdpServerSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
		if(iReadData > 0){
			QBuffer buffer(&datagram);
			if(buffer.open(QIODevice::ReadOnly)) {
				doReadMsg(&buffer);
				buffer.close();
			}
		}
	}
}

void MemOpRcptServer::run()
{
	bool bGoOn;

	if(m_bUseTCP) {
		m_pTcpServer = new QTcpServer();
		connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

		qCritical("[aleakd-server] Creating TCP server on port: %d", m_iPort);
		bGoOn = m_pTcpServer->listen(QHostAddress::Any, m_iPort);
	}else {
		m_pUdpServerSocket = new QUdpSocket();
		connect(m_pUdpServerSocket, SIGNAL(readyRead()), this, SLOT(onPendingDatagramToRead()));

		qCritical("[aleakd-server] Creating UDP server on port: %d", m_iPort);
		bGoOn = m_pUdpServerSocket->bind(QHostAddress::Any, m_iPort);
	}
	if(!bGoOn){
		qCritical("[aleakd-server] Cannot initialize service");
	}

	exec();

	if(m_pTcpServer){
		delete m_pTcpServer;
		m_pTcpServer = NULL;
	}
	if(m_pUdpServerSocket){
		delete m_pUdpServerSocket;
		m_pUdpServerSocket = NULL;
	}
}