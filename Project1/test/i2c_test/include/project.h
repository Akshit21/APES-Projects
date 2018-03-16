#ifndef _PROJECT_H__
#define _PROJECT_H__
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
#include <netinet/in.h>
#include <time.h>
#include <mqueue.h>
#include <sys/types.h>


#include "task.h"
#include "user_i2c.h"
#include "apds9301.h"
#include "tmp1021.h"
#include "helper.h"
#include "api.h"
#include "led.h"
#include "myqueue.h"

#define SENSOR_STARTUP_TEST
#define TEMP_TASK
#define TEMP_TASK_ALERT
#define LIGHT_TASK
#define LIGHT_TASK_ALERT
#define SOCKET_TASK

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

#endif
