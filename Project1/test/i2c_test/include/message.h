/***************************************************************************************************
* Copyright (C) 2017 by Akshit Shah, Shuting Guo
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Akshit Shah, Shuting Guo, Prof Alex Fosdick and the University of Colorado are
* not liable for any misuse of this material.
***************************************************************************************************/
/***************************************************************************************************
* @author : Akshit Shah
* @date : 02/22/2018
*
* @file : api.h
* @brief : This header file provides an abstraction of thread definations and initialization
           variables.
***************************************************************************************************/
#ifndef API_H_
#define API_H_

#define MAX_PAYLOAD_SIZE (20)

#define LOGGER_QUEUE	"/qlogger"
#define TEMP_QUEUE		"/qtemp"
#define LIGHT_QUEUE		"/qlight"
#define MAIN_QUEUE		"/qmain"
#define SOCKET_QUEUE	"/qsocket"
#define QUEUE_SIZE		(1024)

typedef enum _Status_t
{
	ERROR,
  SUCCESS,

  /*Add new states above this line*/
  ERROR_MAX
} Status_t;

typedef enum _RequesId_t
{
    LOG_MSG,
    GET_TEMP_C,
		GET_TEMP_K,
		GET_TEMP_F,
    GET_LIGHT,
    GET_LIGHT_STATE,
    SHUT_DOWN,
		HEART_BEAT,
    STARTUP_TEST,

    /*Add new states above this line*/
    REQUEST_MAX
} RequestId_t;

typedef enum _Source_t
{
    MAIN_THREAD,
    LOGGER_THREAD,
    TEMP_THREAD,
    LIGHT_THREAD,
    SOCKET_THREAD,
		SOCKET_CLIENT,

    /*Add new states above this line*/
    SOURCE_MAX
} Source_t;

typedef enum _Dest_t
{
    MAINTHREAD,
    LOGGERTHREAD,
    TEMPTHREAD,
    LIGHTTHREAD,
    SOCKETTHREAD,
		SOCKETCLIET,

    /*Add new states above this line*/
    DEST_MAX
} Dest_t;

typedef enum _LogLevel_t
{
    INFO,
    WARNING,
    ERROR_MSG,
    HEARTBEAT,
    INIT,

    /*Add new states above this line*/
    LEVEL_MAX
} LogLevel_t;

typedef struct _Message_t
{
    Source_t sourceId;
    Dest_t destId;
    LogLevel_t type;
    RequestId_t requestId;
    time_t timeStamp;
    char msg[MAX_PAYLOAD_SIZE];
} Message_t;

typedef struct _ThreadInfo_t
{
	Message_t data;
	pthread_mutex_t thread_mutex_lock;
	//unsigned int priority;
	const char *qName;
} ThreadInfo_t;


extern pthread_mutex_t log_queue_mutex;
extern pthread_mutex_t temp_queue_mutex;
extern pthread_mutex_t main_queue_mutex;
extern pthread_mutex_t light_queue_mutex;
extern pthread_mutex_t socket_queue_mutex;

extern int32_t log_queue_flag;
extern int32_t temp_queue_flag;
extern int32_t light_queue_flag;
extern int32_t socket_queue_flag;
extern int32_t main_queue_flag;

extern char *task[];
extern char *levels[];
/*FUNCTION PROTOTYPES*/
Message_t create_message_struct(Source_t src, Dest_t dest, LogLevel_t level,
							RequestId_t req);
void update_queue_flag(Dest_t dest);
Status_t msg_send(ThreadInfo_t *info);
Status_t msg_receive(ThreadInfo_t *info);
Status_t msg_log(ThreadInfo_t *info);

//Status_t request_heartbeat();

#endif /* API_H_ */
