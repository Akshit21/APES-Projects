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
* @file : main.h
* @brief : This header file provides an abstraction of thread definations and initialization 
           variables.
***************************************************************************************************/
#ifndef LOGGER_H_
#define LOGGER_H_

#define MAX_PAYLOAD_SIZE (256)

typedef enum _Status_t
{
    SUCCESS,
    ERROR_READ,
    ERROR_WRITE,
    ERROR_OPEN,
    ERROR_ADDR,
    ERROR_DATA,
    NULL_PTR
} Status_t;


typedef enum _RequesId_t
{
    HEARTBEAT,
    STARTUP_TEST,
    INIT,
    LOG_MSG,
    GET_TEMP,
    GET_LIGHT,
    GET_LUX,
    CLOSE_THREAD
} RequesId_t;


typedef enum _Source_t
{
    MAIN_THREAD,
    LOGGER_THREAD,
    TEMP_THREAD,
    LIGHT_THREAD,
    SOCKET_THREAD
} Source_t;


typedef enum _Destination_t
{
    MAIN_THREAD,
    LOGGER_THREAD,
    TEMP_THREAD,
    LIGHT_THREAD,
    SOCKET_THREAD
} Destination_t;

typedef enum _LogType_t
{
    INFO,
    WARNING,
    ERROR,
    HEARTBEAT,
    INIT
} LogType_t;

typedef struct _Message_t
{
    Source_t sourceId;
    Dest_t destId;
    LogType_t logtype;
    RequesId_t requestId;
    time_t timeStamp;
    Status_t status;
    char dataPayload[MAX_PAYLOAD_SIZE];
} Message_t;

#endif /* LOGGER_H_ */
