#include "../config.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

// socket
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../shared/global-const.h"

#include "server-comm.h"


#define DEFAULT_SERVER_HOST "127.0.0.1"
#define DEFAULT_SERVER_PORT 19999

int g_socket = -1;
struct sockaddr_in g_server;

#ifdef USE_TCP_SERVER
struct hostent *hostinfo = NULL;
#endif

char g_preInitBuffer[4096];
unsigned long g_preInitPos = 0;

int servercomm_init()
{
	int res = 0;

	fprintf(stderr, "[aleakd] init socket\n");
#ifdef USE_TCP_SERVER
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
#else
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
		char* szPort = getenv("ALEAKD_SERVER_HOST");
		if(szPort) {
			g_server.sin_port = htons(atoi(szPort));
		}else{
			g_server.sin_port = htons(DEFAULT_SERVER_PORT);
		}

#ifdef USE_TCP_SERVER
		// Connect to remote server
		if (connect(g_socket, (struct sockaddr *) &g_server, sizeof(g_server)) < 0) {
			fprintf(stderr, "[aleakd] Could not connect socket\n");
			res = -1;
		}
#endif
	}

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
		}
	}

	return res;
}

void servercomm_dispose()
{
	if(g_socket != -1){
		close(g_socket);
	}
}

int servercomm_send(const void* buff, size_t size)
{
#ifdef USE_TCP_SERVER
	if (send(g_socket, buff, size, 0) < 0) {
		fprintf(stderr, "[aleakd] error to send data\n");
		return -1;
	}
#else
	if(sendto(g_socket, buff, size, 0,  (const struct sockaddr *)&g_server, sizeof(g_server)) < 0)
	{
		fprintf(stderr, "[aleakd] error to send data\n");
		return -1;
	}

#endif
	return 0;
}

int servercomm_send_safe(const void* buff, size_t size)
{
	// Store send data if init is not done
	if(g_socket != -1) {
		//fprintf(stderr, "[aleakd] -- send: %d\n", size);
		return servercomm_send(buff, size);
	}else{
		//fprintf(stderr, "[aleakd] -- presend: %d\n", size);
		char* szBuffStart = ((char*)g_preInitBuffer)+g_preInitPos;
		memcpy(szBuffStart, buff, size);
		g_preInitPos += sizeof(struct ServerMsgMemoryV1);
	}
	return size;
}

void servercomm_msg_header_init_v1(struct ServerMsgHeaderV1* pServerMsgHeader)
{
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);

	pServerMsgHeader->time_sec = tvNow.tv_sec;
	pServerMsgHeader->time_usec = tvNow.tv_usec;
	pServerMsgHeader->thread_id = (int64_t)pthread_self();
	pServerMsgHeader->msg_code = ALeakD_MsgCode_unknown;
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
	//fprintf(stderr, "[aleakd] memory msg: %d\n", pServerMemoryMsg->header.msg_code);
	return servercomm_send_safe(pServerMemoryMsg, sizeof(struct ServerMsgMemoryV1));
}

void servercomm_msg_thread_init_v1(struct ServerMsgThreadV1* pServerMsgThread)
{
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);

	pServerMsgThread->msg_version = ALEAKD_MSG_VERSION;

	servercomm_msg_header_init_v1(&pServerMsgThread->header);

	pServerMsgThread->data.thread_id = 0;
	memset(pServerMsgThread->data.thread_name, 0, sizeof(pServerMsgThread->data.thread_name));
}

int servercomm_msg_thread_send_v1(struct ServerMsgThreadV1* pServerMsgThread)
{
	//fprintf(stderr, "[aleakd] thread msg: %d\n", pServerMsgThread->header.msg_code);
	return servercomm_send_safe(pServerMsgThread, sizeof(struct ServerMsgThreadV1));
}