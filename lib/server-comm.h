#ifndef ALEAKD_SERVER_COMM
#define ALEAKD_SERVER_COMM

#include <stdlib.h>
#include <stdint.h>

#include "../shared/server-msg.h"

int servercomm_init();
void servercomm_dispose();
int servercomm_send(const void* buff, size_t size);

void servercomm_msg_init_v1(struct ServerMemoryMsgV1* pServerMemoryMsg);
int servercomm_msg_send_v1(struct ServerMemoryMsgV1* pServerMemoryMsg);

#endif // ALEAKD_SERVER_COMM