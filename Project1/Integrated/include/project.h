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
* @file : project.h
* @brief : This header file provides a common header interface for entire project
***************************************************************************************************/
#ifndef _PROJECT_H__
#define _PROJECT_H__

/* Standard Include */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <mqueue.h>
#include <sys/types.h>

/* Custom Includes */
#include "message.h"
#include "task.h"
#include "user_i2c.h"
#include "apds9301.h"
#include "tmp1021.h"
#include "helper.h"
#include "led.h"

#define _GNU_SOURCE


#define SENSOR_STARTUP_TEST
/* Compile time switch for debugging */
#define TEMP_TASK
//#define TEMP_TASK_ALERT
#define TEMP_TASK_MESSAGING
#define LIGHT_TASK
#define LIGHT_TASK_ALERT
#define LIGHT_TASK_MESSAGING
#define SOCKET_TASK
#define LOGGER_TASK
#define LOGGER_TASK_MESSAGING
//#define MAIN_HEARTBEAT_REQUEST
#define DEBUG printf

#endif /* _PROJECT_H__ */
