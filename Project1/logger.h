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
* @file : logger.h
* @brief : This header file provides an abstraction of thread definations and initialization 
           variables.
***************************************************************************************************/
#ifndef LOGGER_H_
#define LOGGER_H_

#define MAX_PAYLOAD_SIZE (10)

typedef enum _Status_t
{
    SUCCESS,
    ERROR_READ,
    ERROR_WRITE,
    ERROR_OPEN,
	ERROR_CLOSE,
    ERROR_ADDR,
    ERROR_DATA,
    NULL_PTR
} Status_t;


typedef enum _RequesId_t
{
    HEART_BEAT,
    STARTUP_TEST,
    LOG_MSG,
    GET_TEMP,
    GET_LIGHT,
    GET_LUX,
    CLOSE_SYSTEM
} RequesId_t;


typedef enum _Source_t
{
    MAIN_THREAD,
    LOGGER_THREAD,
    TEMP_THREAD,
    LIGHT_THREAD,
    SOCKET_THREAD
} Source_t;


typedef enum _LogLevel_t
{
    INFO,
    WARNING,
    ERROR,
    HEARTBEAT,
    INIT
} LogLevel_t;


typedef struct _Message_t
{
    Source_t sourceId;
    LogLevel_t type;
    RequesId_t requestId;
    time_t timeStamp;
    char msg[MAX_PAYLOAD_SIZE];
} Message_t;

Status_t create_message_info(Message_t **message);
Status_t log_data(FILE **pfile, Message_t *message);
Status_t get_log_file(FILE **pfile, char *fileName);

#endif /* LOGGER_H_ */
