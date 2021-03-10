//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_SERVER_MSG_H
#define ALEAKD_SERVER_MSG_H

struct ServerMemoryMsgV1
{
	uint8_t version;

	// Msg timestamp
	uint64_t time_sec;
	uint64_t time_usec;

	// Msg type
	uint8_t msg_type; // ALeakD_AllocType

	// Current thread
	uint64_t thread_id;

	// Alloc infos
	uint64_t alloc_size;
	uint64_t alloc_ptr;
	uint32_t alloc_num;
	// Free infos
	uint64_t free_ptr;

} _ServerMemoryMsgV1;

#endif //ALEAKD_SERVER_MSG_H
