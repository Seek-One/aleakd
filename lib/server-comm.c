#include "../config.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// socket
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../shared/global-const.h"

#include "backtrace.h"

#include "server-comm.h"

#define DEFAULT_SERVER_HOST "127.0.0.1"
#define DEFAULT_SERVER_PORT 19999

int g_bInit = 0;
int g_socket = -1;
struct sockaddr_in g_server;
pthread_mutex_t g_lockSocket = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t g_lockMsgCount = PTHREAD_MUTEX_INITIALIZER;
int g_iMsgNum = 0;

#define BUFFER_SIZE 4096
int g_bTransfertBufferEnabled = 0;
char g_pTransferBuffer[BUFFER_SIZE];
unsigned long g_iTransfertBufferSize = 0;

char g_preInitBuffer[BUFFER_SIZE];
unsigned long g_preInitPos = 0;

int servercomm_send_physical(const void* buff, size_t size);

int servercomm_init_socket()
{
	int res = 0;

#if TRANSFER_MODE_USED == TRANSFER_MODE_TCP
	fprintf(stderr, "[aleakd] init comm with mode: tcp\n");
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
#endif
#if TRANSFER_MODE_USED == TRANSFER_MODE_UDP
	fprintf(stderr, "[aleakd] init comm with mode: udp\n");
	g_socket = socket(AF_INET, SOCK_DGRAM, 0);
#endif
	if (g_socket == -1)
	{
		fprintf(stderr, "[aleakd] Could not create socket\n");
		res = -1;
	}

	if(res == 0) {
		char* szHost = getenv("ALEAKD_SERVER_HOST");
		if(szHost){
			g_server.sin_addr.s_addr = inet_addr(szHost);
		}else {
			g_server.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_HOST);
		}
		g_server.sin_family = AF_INET;
		char* szPort = getenv("ALEAKD_SERVER_PORT");
		if(szPort) {
			g_server.sin_port = htons(atoi(szPort));
		}else{
			g_server.sin_port = htons(DEFAULT_SERVER_PORT);
		}

#if TRANSFER_MODE_USED == TRANSFER_MODE_TCP
		// Connect to remote server
		if (connect(g_socket, (struct sockaddr *) &g_server, sizeof(g_server)) < 0) {
			fprintf(stderr, "[aleakd] Could not connect socket\n");
			res = -1;
		}
#endif
	}

	return res;
}

int servercomm_name_pipe()
{
	int res = 0;

	fprintf(stderr, "[aleakd] init comm with mode: named pipe\n");

	const char* szFile = NAMED_PIPE_FILE;

	res = access( szFile, F_OK );
	if(res == 0 ) {
		g_socket = open(szFile, O_WRONLY);
		if(g_socket <= 0){
			res = -1;
		}
	} else {
		fprintf(stderr, "[aleakd] named pipe not found %s\n", NAMED_PIPE_FILE);
	}

	return res;
}

int servercomm_init()
{
	int res = 0;

#if TRANSFER_MODE_USED == TRANSFER_MODE_NAMEDPIPE
	res = servercomm_name_pipe();
#else
	res = servercomm_init_socket();
#endif

	// Send protocol version
	if(res == 0) {
		struct ServerMsgAppV1 msg;
		servercomm_msg_app_init_v1(&msg);
		res = servercomm_msg_app_send_v1(&msg);
	}

	// Send some data
	if(res == 0) {
		if(g_preInitPos > 0) {
			fprintf(stderr, "[aleakd] sending pre init data: %d\n", g_preInitPos);
			res = servercomm_send((void *) g_preInitBuffer, g_preInitPos);
			g_preInitPos = 0;
		}
	}

#ifdef BUFFERIZE_TRANSFERT
	g_bTransfertBufferEnabled = 1;
#endif
	g_bInit = 1;

	return res;
}

void servercomm_dispose()
{
	if(g_socket != -1){
		if(g_bTransfertBufferEnabled) {
			fprintf(stderr, "[aleakd] sending last transfert buffer data: %lu bytes\n", g_iTransfertBufferSize);
			servercomm_send_physical(g_pTransferBuffer, g_iTransfertBufferSize);
			g_bTransfertBufferEnabled = 0;
		}
		fprintf(stderr, "[aleakd] dispose socket, total message: %d\n", g_iMsgNum);
		// Don't close socket because more message can arrive
		//close(g_socket);
		//g_socket == -1;
		fprintf(stderr, "[aleakd] some message can be sent after this point\n");
		g_bInit = 0;
	}
}

int servercomm_send_physical(const void* buff, size_t size)
{
	int res = 0;

	//fprintf(stderr, "[aleakd] servercomm_send_physical #%d (%lu bytes)\n", g_iMsgNum, size);
//	char szData[9000];
//	ALeakD_toHexString(buff, size, szData, 9000);
//	fprintf(stderr, "[aleakd] servercomm_send_physical #%s\n", szData);

#if TRANSFER_MODE_USED == TRANSFER_MODE_TCP
	res = send(g_socket, buff, size, 0);
#endif
#if TRANSFER_MODE_USED == TRANSFER_MODE_UDP
	res = sendto(g_socket, buff, size, 0,  (const struct sockaddr *)&g_server, sizeof(g_server));
#endif
#if TRANSFER_MODE_USED == TRANSFER_MODE_NAMEDPIPE
	res = write(g_socket, buff, size);
#endif
	if(res < 0){
		fprintf(stderr, "[aleakd] error to send data\n");
		return -1;
	}

	return res;
}

int servercomm_send(const void* buff, size_t size)
{
	int res = 0;

	pthread_mutex_lock(&g_lockSocket);

	if(g_bTransfertBufferEnabled) {
		//fprintf(stderr, "[aleakd] servercomm_send: %d\n", size);

		int bSendBuffer = 0;
		if ((g_iTransfertBufferSize + size) >= BUFFER_SIZE) {
			bSendBuffer = 1;
		}

		if (bSendBuffer) {
			res = servercomm_send_physical(g_pTransferBuffer, g_iTransfertBufferSize);
			g_iTransfertBufferSize = 0;
		}

		char *szBuffStart = ((char *) g_pTransferBuffer) + g_iTransfertBufferSize;
		memcpy(szBuffStart, buff, size);
		g_iTransfertBufferSize += size;
	}else {
		servercomm_send_physical(buff, size);
	}

	pthread_mutex_unlock(&g_lockSocket);
	return 0;
}

int servercomm_send_safe(const void* buff, size_t size)
{
	int res = 0;

	if(g_bInit == 0){
		struct ServerMsgHeaderV1 headers;
		memcpy(&headers, buff+sizeof(servermsg_version_t), sizeof(headers));
		fprintf(stderr, "[aleakd] message sent in state unitialized: #%d type:%d (%lu bytes)\n", headers.msg_num, headers.msg_code, size);
		//fprintf(stderr, "[aleakd] message #%d sent (%lu bytes): thread:%lu, msg_code=%d\n", g_iMsgNum, size, headers.thread_id, headers.msg_code);
	}

	if(g_socket != -1) {
		res = servercomm_send((void*)buff, size);
	}else{
		char *szBuffStart = ((char *) g_preInitBuffer) + g_preInitPos;
		memcpy(szBuffStart, buff, size);
		g_preInitPos += size;
	}

	return res;
}

void servercomm_msg_header_init_v1(struct ServerMsgHeaderV1* pServerMsgHeader)
{
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);

	pServerMsgHeader->time_sec = tvNow.tv_sec;
	pServerMsgHeader->time_usec = tvNow.tv_usec;
	pServerMsgHeader->thread_id = (int64_t)pthread_self();
	pServerMsgHeader->msg_code = ALeakD_MsgCode_init;

	// Compute message num
	pthread_mutex_lock(&g_lockMsgCount);
	g_iMsgNum++;
	pServerMsgHeader->msg_num = g_iMsgNum;
	pthread_mutex_unlock(&g_lockMsgCount);

}

void servercomm_msg_app_init_v1(struct ServerMsgAppV1* pServerMsgApp)
{
	pServerMsgApp->msg_version = ALEAKD_MSG_VERSION;
	servercomm_msg_header_init_v1(&pServerMsgApp->header);
}

int servercomm_msg_app_send_v1(struct ServerMsgAppV1* pServerMsgApp)
{
	return servercomm_send_safe(pServerMsgApp, sizeof(struct ServerMsgAppV1));
}

void servercomm_msg_memory_init_v1(struct ServerMsgMemoryV1* pServerMemoryMsg)
{
	pServerMemoryMsg->msg_version = ALEAKD_MSG_VERSION;

	servercomm_msg_header_init_v1(&pServerMemoryMsg->header);

	pServerMemoryMsg->data.alloc_size = 0;
	pServerMemoryMsg->data.alloc_ptr = 0;
	pServerMemoryMsg->data.alloc_num = 0;
	pServerMemoryMsg->data.free_ptr = 0;
}

int servercomm_msg_memory_send_v1(struct ServerMsgMemoryV1* pServerMemoryMsg)
{
	//fprintf(stderr, "[aleakd] memory msg: %d %lu\n", pServerMemoryMsg->header.msg_code, pServerMemoryMsg->header.thread_id);
	return servercomm_send_safe(pServerMemoryMsg, sizeof(struct ServerMsgMemoryV1));
}

void servercomm_msg_thread_init_v1(struct ServerMsgThreadV1* pServerMsgThread)
{
	pServerMsgThread->msg_version = ALEAKD_MSG_VERSION;

	servercomm_msg_header_init_v1(&pServerMsgThread->header);

	pServerMsgThread->data.thread_id = 0;
	memset(pServerMsgThread->data.thread_name, 0, sizeof(pServerMsgThread->data.thread_name));
}

int servercomm_msg_thread_send_v1(struct ServerMsgThreadV1* pServerMsgThread)
{
	//fprintf(stderr, "[aleakd] thread msg: %d %lu\n", pServerMsgThread->header.msg_code, pServerMsgThread->header.thread_id);
	return servercomm_send_safe(pServerMsgThread, sizeof(struct ServerMsgThreadV1));
}

void servercomm_msg_backtrace_init_v1(struct ServerMsgBacktraceV1* pServerMsgBacktrace)
{
	pServerMsgBacktrace->msg_version = ALEAKD_MSG_VERSION;

	servercomm_msg_header_init_v1(&pServerMsgBacktrace->header);
	pServerMsgBacktrace->header.msg_code = ALeakD_MsgCode_backtrace;

	pServerMsgBacktrace->data.origin_msg_num = 0;
	pServerMsgBacktrace->data.backtrace_size = 0;
	memset(pServerMsgBacktrace->data.list_addr, 0, sizeof(pServerMsgBacktrace->data.list_addr));
}

int servercomm_msg_backtrace_send_v1(struct ServerMsgBacktraceV1* pServerMsgBacktrace)
{
	return servercomm_send_safe(pServerMsgBacktrace, sizeof(struct ServerMsgBacktraceV1));
}

void servercomm_msg_backtrace_make(struct ServerMsgBacktraceV1* pBacktrace, void** listBackTraceAddr, int iSize)
{
	pBacktrace->data.backtrace_size = iSize;
	for(int i=0; i<iSize; i++)
	{
		void* addr = listBackTraceAddr[i];
		pBacktrace->data.list_addr[i] = (uint64_t)addr;
	}
}

void servercomm_msg_symbolinfos_init_v1(struct ServerMsgSymbolInfosV1* pServerMsgSymbolInfos)
{
	pServerMsgSymbolInfos->msg_version = ALEAKD_MSG_VERSION;

	servercomm_msg_header_init_v1(&pServerMsgSymbolInfos->header);
	pServerMsgSymbolInfos->header.msg_code = ALeakD_MsgCode_symbolinfos;

	pServerMsgSymbolInfos->data.object_addr = 0;
	memset(pServerMsgSymbolInfos->data.object_name, 0, sizeof(pServerMsgSymbolInfos->data.object_name));
	pServerMsgSymbolInfos->data.symbol_addr = 0;
	memset(pServerMsgSymbolInfos->data.symbol_name, 0, sizeof(pServerMsgSymbolInfos->data.symbol_name));
}

int servercomm_msg_symbolinfos_send_v1(struct ServerMsgSymbolInfosV1* pServerMsgSymbolInfos)
{
	return servercomm_send_safe(pServerMsgSymbolInfos, sizeof(struct ServerMsgSymbolInfosV1));
}
