#ifndef ALEAKD_SERVER_COMM
#define ALEAKD_SERVER_COMM

#include <stdlib.h>
#include <stdint.h>

#include "../shared/server-msg.h"

int servercomm_init();
void servercomm_dispose();
int servercomm_send(const void* buff, size_t size);
int servercomm_send_safe(const void* buff, size_t size);

void servercomm_msg_app_init_v1(struct ServerMsgAppV1* pServerMsgApp);
int servercomm_msg_app_send_v1(struct ServerMsgAppV1* pServerMsgApp);

void servercomm_msg_memory_init_v1(struct ServerMsgMemoryV1* pServerMsgMemory);
int servercomm_msg_memory_send_v1(struct ServerMsgMemoryV1* pServerMsgMemory);

void servercomm_msg_thread_init_v1(struct ServerMsgThreadV1* pServerMsgThread);
int servercomm_msg_thread_send_v1(struct ServerMsgThreadV1* pServerMsgThread);

void servercomm_msg_backtrace_init_v1(struct ServerMsgBacktraceV1* pServerMsgBacktrace);
int servercomm_msg_backtrace_send_v1(struct ServerMsgBacktraceV1* pServerMsgBacktrace);
void servercomm_msg_backtrace_make(struct ServerMsgBacktraceV1* pBacktrace, void** listBackTraceAddr, int iSize);

void servercomm_msg_symbolinfos_init_v1(struct ServerMsgSymbolInfosV1* pServerMsgSymbolInfos);
int servercomm_msg_symbolinfos_send_v1(struct ServerMsgSymbolInfosV1* pServerMsgSymbolInfos);

#endif // ALEAKD_SERVER_COMM