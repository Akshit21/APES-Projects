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
#ifndef MAIN_H_
#define MAIN_H_

/********************************************
* S T A N D A R D    I N C L U D E S
*********************************************/
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<mqueue.h>
#include<sys/stat.h>
#include<errno.h>
#include<string.h>
#include<time.h>
#include<sys/signal.h>
#include<sys/time.h>
#include<stdint.h>
#include<sys/types.h>

/********************************************
* L O C A L   I N C L U D E S
*********************************************/
#include "logger.h"
#include "led.h"
#include "myqueue.h"

/********************************************
* D E F I N E S
*********************************************/
#define NUM_THREADS   (4)
#define LOG_THREAD    (0)
#define TEMP_THREAD   (1)
#define LIGHT_THREAD  (2)
#define SOCKET_THREAD (3)

/********************************************
* V A R I A B L E S
*********************************************/
/* Array to store thread ids */
pthread_t threads[NUM_THREADS];

/********************************************
* T H R E A D  F U N C T I O N S
*********************************************/
void *logThread(void *threadArgs);
void *tempThread(void *threadArgs);
void *lightThread(void *threadArgs);
void *socketThread(void *threadArgs);

#endif  /* MAIN_H_ */
