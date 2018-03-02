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
* @file : myqueue.h
* @brief : This header file provides an abstraction of Queue APIs and initialization 
           variables.
* @ref : https://stackoverflow.com/questions/3056307/how-do-i-use-mqueue-in-a-c-program-on-a-linux-based-system
***************************************************************************************************/
#ifndef MYQUEUE_H_
#define MYQUEUE_H_

#define LOGGER_QUEUE	("/QLogger")
#define TEMP_QUEUE	("/QTemp")
#define LIGHT_QUEUE	("/QLight")
#define MAIN_QUEUE	("/QMain")
#define SOCKET_QUEUE	("/QSocket")
#define QUEUE_SIZE	(1024)

/*synchronisation variables */
pthread_mutex_t log_queue_mutex;
pthread_mutex_t temp_queue_mutex;
pthread_mutex_t main_queue_mutex;
pthread_mutex_t light_queue_mutex;
pthread_mutex_t socket_queue_mutex;

/*Queue Handler variables*/
mqd_t log_queue_handle;
mqd_t temp_queue_handle;
mqd_t light_queue_handle;
mqd_t main_queue_handle;
mqd_t socket_queue_handle;

#endif /* MYQUEUE_H_ */
