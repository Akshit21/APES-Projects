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
* @author : Akshit Shah, Shuting Guo
* @date : 02/22/2018
*
* @file : api.h
* @brief : This header file provides an abstraction of thread definations and initialization
           variables.
***************************************************************************************************/
#ifndef API_H_
#define API_H_

#define MAX_PAYLOAD_SIZE (50)

/* Queues */
#define LOGGER_QUEUE	"/qlogger"
#define TEMP_QUEUE		"/qtemp"
#define LIGHT_QUEUE		"/qlight"
#define MAIN_QUEUE		"/qmain"
#define SOCKET_QUEUE	"/qsocket"
#define QUEUE_SIZE		(1024)

/* Status */
typedef enum _Status_t
{
	ERROR,
  SUCCESS,

  /*Add new states above this line*/
  ERROR_MAX
} Status_t;

/* Requests */
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

/* Sources */
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

/* Destinations */
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

/* LOG Types */
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

/* Message Structure */
typedef struct _Message_t
{
    Source_t sourceId;
    Dest_t destId;
    LogLevel_t type;
    RequestId_t requestId;
    time_t timeStamp;
    char msg[MAX_PAYLOAD_SIZE];
} Message_t;

/* Thread Info Structure */
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

/**
* @brief Create a message structure for queue
*
* Given the parameters of the message structures,
* create a message structure and return it to calling
* thread which can be used to send or receive msg from
* the queue
*
* @param src Source Id of the calling thread
* @param dest Destination Id of the receiving thread
* @param level Log type if any
* @param req Request Id of the message request
*
* @return Message structure variable
*/
Message_t create_message_struct(Source_t src, Dest_t dest,
																LogLevel_t level, RequestId_t req);

/**
* @brief Update the queue flags
*
* Given the Destination id of the queue, update the corresponding
* flag indicating number of message in the queues
*
* @param dest Destination Id of the receiving thread
*
* @return void
*/
void update_queue_flag(Dest_t dest);

/**
* @brief Send the message to the corresponding queue
*
* Given the Thread Info structure which contains the messages
* structure, the mutex and qName. This function sends the message to
* the queue specified by the queue name in the info structure
*
* @param info Pointer to the ThreadInfo_t structure
*
* @return Status SUCCESS/ERROR
*/
Status_t msg_send(ThreadInfo_t *info);

/**
* @brief Receive the message to the corresponding queue
*
* Given the Thread Info structure which contains the messages
* structure, the mutex and qName. This function receives the message to
* the queue specified by the queue name in the info structure and
* stores the message in the message structure pointed by info
*
* @param info Pointer to the ThreadInfo_t structure
*
* @return Status SUCCESS/ERROR
*/
Status_t msg_receive(ThreadInfo_t *info);

/**
* @brief Send the message to the log queue
*
* Wrapper around message send
*
* @param info Pointer to the ThreadInfo_t structure
*
* @return Status SUCCESS/ERROR
*/
Status_t msg_log(ThreadInfo_t *info);

/**
* @brief Send HeartBeat request to all threads
*
* Main uses this helper function to send heart beat
* requests to all the threads
*
* @param void
*
* @return Status SUCCES/ERROR
*/
Status_t request_heartbeat(void);

/**
* @brief Log the data into a local file
*
* Given the File handler, file name and the message to be
* logged, this function formats the data and logs the data in
* the file
*
* @param info Pointer to the ThreadInfo_t structure
*
* @return Status SUCCESS
*/
Status_t log_data(FILE **pfile, Message_t *message, const char *fileName);

#endif /* API_H_ */
