//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_SERVER_MSG_H
#define ALEAKD_SERVER_MSG_H

/////////////////////
// Header structure
/////////////////////

typedef uint16_t servermsg_version_t;

struct __attribute__((__packed__)) ServerMsgHeaderV1
{
	// Msg timestamp
	uint64_t time_sec;
	uint64_t time_usec;

	// Msg thread
	uint64_t thread_id;

	// Msg type
	uint32_t msg_num;
	uint8_t msg_code;
} _ServerMsgHeaderV1;

///////////////////////
// Application message
///////////////////////

struct __attribute__((__packed__)) ServerMsgAppV1
{
	servermsg_version_t msg_version;
	struct ServerMsgHeaderV1 header;
} _ServerMsgAppV1;

///////////////////////
// Memory message
///////////////////////

struct __attribute__((__packed__)) ServerMsgMemoryDataV1
{
	// Alloc infos
	uint64_t alloc_size;
	uint64_t alloc_ptr;
	uint32_t alloc_num;
	// Free infos
	uint64_t free_ptr;

} _ServerMsgMemoryDataV1;

struct __attribute__((__packed__)) ServerMsgMemoryV1
{
	servermsg_version_t msg_version;
	struct ServerMsgHeaderV1 header;
	struct ServerMsgMemoryDataV1 data;
} _ServerMemoryMsgV1;

///////////////////////
// Thread message
///////////////////////

struct __attribute__((__packed__)) ServerMsgThreadDataV1
{
	// Thread
	uint64_t thread_id;
	// Free infos
	char thread_name[24];
};

struct __attribute__((__packed__)) ServerMsgThreadV1
{
	servermsg_version_t msg_version;
	struct ServerMsgHeaderV1 header;
	struct ServerMsgThreadDataV1 data;
} _ServerMsgThreadV1;

///////////////////////
// Backtrace message
///////////////////////

struct __attribute__((__packed__)) ServerMsgBacktraceRowV1
{
	uint64_t addr;
	uint16_t symbol_name_size;
	const char* symbol_name;
	uint16_t object_name_size;
	const char* object_name;
} _ServerMsgBacktraceRowV1;

struct __attribute__((__packed__)) ServerMsgBacktraceDataV1
{
	uint16_t origin_msg_num;
	uint8_t backtrace_size;
	uint64_t list_addr[BACKTRACE_MAX_SIZE];
} _ServerMsgBacktraceDataV1;

struct __attribute__((__packed__)) ServerMsgBacktraceV1
{
	servermsg_version_t msg_version;
	struct ServerMsgHeaderV1 header;
	struct ServerMsgBacktraceDataV1 data;
} _ServerMsgBacktraceV1;

#endif //ALEAKD_SERVER_MSG_H
