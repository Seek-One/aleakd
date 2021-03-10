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

int g_socket = -1;
#define DEFAULT_SERVER_HOST "127.0.0.1"
#define DEFAULT_SERVER_PORT 19999

char g_preInitBuffer[4096];
unsigned long g_preInitPop = 0;

int servercomm_init()
{
	int res = 0;

	fprintf(stderr, "[aleakd] init socket\n");
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket == -1)
	{
		fprintf(stderr, "[aleakd] Could not create socket\n");
		res = -1;
	}

	if(res == 0) {
		struct sockaddr_in server;
		char* szHost = getenv("ALEAKD_SERVER_HOST");
		if(szHost){
			server.sin_addr.s_addr = inet_addr(szHost);
		}else {
			server.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_HOST);
		}
		server.sin_family = AF_INET;
		char* szPort = getenv("ALEAKD_SERVER_HOST");
		if(szPort) {
			server.sin_port = htons(atoi(szPort));
		}else{
			server.sin_port = htons(DEFAULT_SERVER_PORT);
		}

		// Connect to remote server
		if (connect(g_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
			fprintf(stderr, "[aleakd] Could not connect socket\n");
			res = -1;
		}
	}

	// Send some data
	if(res == 0) {
		if(g_preInitPop > 0) {
			res = servercomm_send((void *) g_preInitBuffer, g_preInitPop);
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
	if (send(g_socket, buff, size, 0) < 0) {
		fprintf(stderr, "[aleakd] error to send data\n");
		return -1;
	}
	return 0;
}

void servercomm_msg_init_v1(struct ServerMemoryMsgV1* pServerMemoryMsg)
{
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);

	pServerMemoryMsg->version = ALEAKD_MSG_VERSION;
	pServerMemoryMsg->time_sec = tvNow.tv_sec;
	pServerMemoryMsg->time_usec = tvNow.tv_usec;

	pServerMemoryMsg->msg_type = 0;

	pServerMemoryMsg->thread_id = (int64_t)pthread_self();

	pServerMemoryMsg->alloc_size = 0;
	pServerMemoryMsg->alloc_ptr = 0;
	pServerMemoryMsg->alloc_num = 0;

	pServerMemoryMsg->free_ptr = 0;
}

int servercomm_msg_send_v1(struct ServerMemoryMsgV1* pServerMemoryMsg)
{
	if(g_socket != -1) {
		fprintf(stderr, "[aleakd] msg: %d\n", pServerMemoryMsg->msg_type);
		return servercomm_send((void *) pServerMemoryMsg, sizeof(struct ServerMemoryMsgV1));
	}else{
		char* szBuffStart = ((char*)g_preInitBuffer)+g_preInitPop;
		memcpy(szBuffStart, pServerMemoryMsg, sizeof(struct ServerMemoryMsgV1));
		g_preInitPop += sizeof(struct ServerMemoryMsgV1);
	}
}