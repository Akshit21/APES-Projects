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

typedef enum _Status_t
{
	ERROR,
    SUCCESS, 
    
    /*Add new states above this line*/
    ERROR_MAX
} Status_t;

typedef enum _RequesId_t
{
    HEART_BEAT,
    STARTUP_TEST,
    LOG_MSG,
    GET_TEMP,
    GET_LIGHT,
    GET_LIGHT_STATE,
    SHUT_DOWN,
    
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
    
    /*Add new states above this line*/
    DEST_MAX
} Dest_t;

typedef enum _LogLevel_t
{
    INFO,
    WARNING,
    ERROR,
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

/*FUNCTION PROTOTYPES*/
Message_t create_message_struct(Source_t src, Dest_t dest, LogLevel_t level,
							RequestId_t req);
void update_flag(Dest_t dest);

Status_t log_data(FILE **pfile, Message_t *message, char *fileName);
Status_t get_log_file(FILE **pfile, char *fileName);
Status_t msg_send(ThreadInfo_t *info);
Status_t msg_receive(ThreadInfo_t *info);
Status_t request_heartbeat();
Status_t msg_log(ThreadInfo_t *info);

#endif /* API_H_ */
