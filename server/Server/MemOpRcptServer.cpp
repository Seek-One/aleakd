//
// Created by ebeuque on 09/03/2021.
//

#include "../../config.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <QDateTime>
#include <QBuffer>
#include <QFile>

#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

#include "Model/MemoryOperation.h"
#include "Model/ThreadOperation.h"

#include "../shared/server-msg.h"
#include "../shared/global-const.h"

#include "MemOpRcptServer.h"

MemOpRcptServer::MemOpRcptServer(QObject* parent)
	: QThread(parent)
{
	m_iPort = 0;

	m_iTransferMode = TRANSFER_MODE_USED;

	m_pTcpClientSocket = NULL;
	m_pUdpServerSocket = NULL;
	m_pNamedPipeFile = NULL;

	m_iMsgCount = 0;
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
		if(doReadMsg(m_pTcpClientSocket)) {
			qDebug("message received: %d", m_iMsgCount);
		}else{
			break;
		}
	} while (true);
}

bool MemOpRcptServer::doReadMsg(QIODevice* pIODevice)
{
	bool bRes = false;

	char pBuffer[8192]; // Read 4096 but use bigger buffer to be able to add remaining data
	int iBufferReadSize = 0;

	qint64 iSizeRead;
	qint64 iSizeProcessed;
	qint64 iSizeRemaining;
	do{
		iSizeRead = pIODevice->read(pBuffer+iBufferReadSize, 4096);

		if(iSizeRead > 0) {
			iBufferReadSize += iSizeRead;

//			char szString[9000];
//			ALeakD_toHexString((const unsigned char*)pBuffer, iBufferReadSize, szString, 9000);
//			qDebug("read: %d", iBufferReadSize);
//			qDebug("read: %s", szString);

			iSizeProcessed = doProcessDataRead(pBuffer, iBufferReadSize);
			if(iSizeProcessed > 0) {
				iSizeRemaining = iBufferReadSize - iSizeProcessed;
				if (iSizeRemaining > 0) {
					memcpy(pBuffer, pBuffer + iSizeProcessed, iSizeRemaining);
					iBufferReadSize = iSizeRemaining;
				} else {
					iBufferReadSize = 0;
				}
			}else{
				qCritical("[aleakd-server] Error to process data");
				break;
			}
		}
	}while(iSizeRead);

	return bRes;
}

qint64 MemOpRcptServer::doProcessDataRead(char* pBuffer, qint64 iMaxSize)
{
	int iRes = 0;
	int iSizeRead = 0;

	do
	{
		if(iSizeRead < iMaxSize) {
			iRes = doProcessMsg(pBuffer + iSizeRead, iMaxSize - iSizeRead);
			if (iRes > 0) {
				iSizeRead += iRes;
				m_iMsgCount++;
				//qDebug("message: %d (%ld bytes)", m_iMsgCount, iRes);
			} else if (iRes == 0) {
				// End of data we can read
				return iSizeRead;
			} else {
				return iRes;
			}
		}else{
			return iSizeRead;
		}
	}while(iRes > 0);

	return iRes;
}

qint64 MemOpRcptServer::doProcessMsg(char* pBuffer, qint64 iMaxSize)
{
	int iRes = 0;
	qint64 iSizeRead = 0;

	if(iMaxSize < sizeof(servermsg_version_t)){
		return 0;
	}

	// Read message version to decide how to process the message
	servermsg_version_t iMsgVersion;
	memcpy(&iMsgVersion, pBuffer, sizeof(servermsg_version_t));
	iSizeRead += sizeof(servermsg_version_t);

	if(iMsgVersion == 1){
		iRes = doProcessMsgV1(pBuffer+iSizeRead, iMaxSize-iSizeRead);
		if(iRes > 0){
			return iSizeRead + iRes;
		}else{
			return iRes;
		}
	}

	return -1;
}

qint64 MemOpRcptServer::doProcessMsgV1(char* pBuffer, qint64 iMaxSize)
{
	int iSizeRead = 0;

	// Check if we can read header size
	if(iMaxSize < sizeof(ServerMsgHeaderV1)){
		return 0;
	}

	// Set headers
	ServerMsgHeaderV1 header;
	memcpy(&header, pBuffer, sizeof(header));
	iSizeRead += sizeof(header);

	//qDebug("msg header: %d %lu", header.msg_code, header.thread_id);

	// Application message
	if (header.msg_code >= 0 && header.msg_code < 10)
	{
		if(header.msg_code == ALeakD_MsgCode_init)
		{
			// Should be the first message
			m_iMsgCount = 0;
			if (m_pHandler) {
				m_pHandler->onNewConnection();
			}
		}
	}

	// Memory operation
	if (header.msg_code >= 10 && header.msg_code < 20)
	{
		if((iMaxSize - iSizeRead) < sizeof(ServerMsgMemoryDataV1)){
			return 0;
		}

		ServerMsgMemoryDataV1 data;
		memcpy(&data, pBuffer+iSizeRead, sizeof(data));
		iSizeRead += sizeof(data);

		MemoryOperation *pMemoryOperation = new MemoryOperation();
		pMemoryOperation->m_tvOperation.tv_sec = header.time_sec;
		pMemoryOperation->m_tvOperation.tv_usec = header.time_usec;
		pMemoryOperation->m_iMsgCode = (ALeakD_MsgCode) header.msg_code;
		pMemoryOperation->m_iCallerThreadId = header.thread_id;
		pMemoryOperation->m_iAllocSize = data.alloc_size;
		pMemoryOperation->m_iAllocPtr = data.alloc_ptr;
		pMemoryOperation->m_iAllocNum = data.alloc_num;
		pMemoryOperation->m_iFreePtr = data.free_ptr;

//		qDebug("[aleakd-server] msg received: type=%d, time=%lu,%lu, thread=%lu, size=%lu",
//			msg.msg_type, msg.time_sec, msg.time_usec,
//		  msg.thread_id, msg.alloc_size);

//		struct timeval tvStart;
//		struct timeval tvRecpt;
//		gettimeofday(&tvStart, NULL);
//		timersub(&tvStart, &pMemoryOperation->m_tvOperation, &tvRecpt);

		if (m_pHandler) {
			m_pHandler->onMemoryOperationReceived(MemoryOperationSharedPtr(pMemoryOperation));
		}

//		struct timeval tvEnd;
//		struct timeval tvProcess;
//		gettimeofday(&tvEnd, NULL);
//		timersub(&tvEnd, &tvStart, &tvProcess);
//		qDebug("[aleakd-latency] latency=%lu,%06lu, process=%lu,%06lu", tvRecpt.tv_sec, tvRecpt.tv_usec,
//		 	tvProcess.tv_sec, tvProcess.tv_usec);
	}


	// Thread operation
	if (header.msg_code >= 30 && header.msg_code < 40)
	{
		if(iMaxSize - iSizeRead < sizeof(ServerMsgThreadDataV1)){
			return 0;
		}

		ServerMsgThreadDataV1 data;
		memcpy(&data, pBuffer+iSizeRead, sizeof(data));
		iSizeRead += sizeof(data);

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

	if(header.backtrace_size > 0){

	}

	return iSizeRead;
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
	bool bGoOn = true;

	if(m_iTransferMode == TRANSFER_MODE_TCP) {
		m_pTcpServer = new QTcpServer();
		connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

		qDebug("[aleakd-server] Creating TCP server on port: %d", m_iPort);
		bGoOn = m_pTcpServer->listen(QHostAddress::Any, m_iPort);
	}
	if(m_iTransferMode == TRANSFER_MODE_UDP) {
		m_pUdpServerSocket = new QUdpSocket();
		connect(m_pUdpServerSocket, SIGNAL(readyRead()), this, SLOT(onPendingDatagramToRead()));

		qDebug("[aleakd-server] Creating UDP server on port: %d", m_iPort);
		bGoOn = m_pUdpServerSocket->bind(QHostAddress::Any, m_iPort);
	}
	if(m_iTransferMode == TRANSFER_MODE_NAMEDPIPE) {
		qDebug("[aleakd-server] Creating named pipe : %s", NAMED_PIPE_FILE);
		m_pNamedPipeFile = new QFile(NAMED_PIPE_FILE);
		if(!m_pNamedPipeFile->exists()) {
			bGoOn = (mkfifo(NAMED_PIPE_FILE, 0666) >= 0);
			if (!bGoOn) {
				qCritical("[aleakd-server] Unable to create named pipe");
			}
		}
	}
	if(!bGoOn){
		qCritical("[aleakd-server] Cannot initialize service");
	}

	if(!m_pNamedPipeFile) {
		exec();
	}else{
		while(bGoOn) {
			bGoOn = m_pNamedPipeFile->open(QIODevice::ReadOnly);
			if (!bGoOn) {
				qCritical("[aleakd-server] Unable to open the named pipe");
			}
			if(bGoOn) {
				qDebug("[aleakd-server] Start named pipe reading");
				while (doReadMsg(m_pNamedPipeFile)) {

				}
				qDebug("[aleakd-server] Stop named pipe reading");
				m_pNamedPipeFile->close();
			}
		}
	}

	if(m_pNamedPipeFile){
		m_pNamedPipeFile->close();
		m_pNamedPipeFile->remove();
		delete m_pNamedPipeFile;
		m_pNamedPipeFile = NULL;
	}

	if(m_pTcpServer){
		delete m_pTcpServer;
		m_pTcpServer = NULL;
	}
	if(m_pUdpServerSocket){
		m_pUdpServerSocket->close();
		delete m_pUdpServerSocket;
		m_pUdpServerSocket = NULL;
	}
}
