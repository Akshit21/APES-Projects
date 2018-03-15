/**************************************************************************************************
* Copyright (C) 2017 by Akshit Shah, Shuting Guo
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Akshit Shah, Shuting Guo, Prof. Alex Fosdick and the University of Colorado are 
* not liable for any misuse of this material.
***************************************************************************************************/
/***************************************************************************************************
* @author : Akshit Shah
* @date : 02/22/2018
*
* @file : myqueue.c
* @brief : Queue and Message APIs
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference : https://stackoverflow.com/questions/3056307/how-do-i-use-mqueue-in-a-c-program-on-a-linux-based-system 
***************************************************************************************************/

#include "main.h"

int flag = 0;

void init_queue()
{
	struct mq_attr attr;
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = sizeof(Message_t);
	attr.mq_flags = 0;

	log_queue_handle =  mq_open(LOGGER_QUEUE,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, &attr);
	temp_queue_handle =  mq_open(TEMP_QUEUE,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, &attr);
	light_queue_handle =  mq_open(LIGHT_QUEUE,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, &attr);
	main_queue_handle =  mq_open(MAIN_QUEUE,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, &attr);
	socket_queue_handle =  mq_open(SOCKET_QUEUE,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, &attr);

	if (log_queue_handle == -1 || temp_queue_handle == -1 ||
		light_queue_handle == -1 || main_queue_handle == -1 ||
		socket_queue_handle == -1)
	{
		printf("Error Opening Queue\nERRNO: %d\n",errno);
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Queue Created successfully\n");
	}
}


void *logThread(void *threadArgs)
{
    printf("In Logger Thread\n");
    
    struct mq_attr attr;
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = sizeof(Message_t);
	attr.mq_flags = 0;

    /* Create a messaging structure */
    Message_t msg={0};
    FILE *pLogfile;
	
	while(1)
	{
		if (flag)
		{
			mqd_t log_queue_handle1 =  mq_open(LOGGER_QUEUE,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, NULL);

			if(mq_receive(log_queue_handle1,(int8_t*)&msg, 1024, 0) != 0)
			{
				printf("Failed to receive any data for logging\n");
			}
			//pthread_mutex_lock(&log_queue_mutex);
			
			if(msg.requestId == LOG_MSG)
			{
				printf ("Source ID: %d \n", msg.sourceId);
				printf ("Timestamp: %s \n", ctime(&msg.timeStamp));
				printf ("Log Level: %d \n", msg.type);
				printf ("Message Data: %s \n", msg.msg);
			}
			
			mq_close(log_queue_handle1);
			break;
			
			//pthread_mutex_unlock(&log_queue_mutex);
		}
	}
}

void *tempThread(void *threadArgs)
{
	printf("In Temp Thread\n");
	//pthread_mutex_lock(&log_queue_mutex);
	Message_t temp_msg;

	temp_msg.sourceId = TEMP_THREAD;
	temp_msg.type = INFO;
	temp_msg.requestId = LOG_MSG;
	temp_msg.timeStamp =  time(NULL);
	char my_msg[] = "Temp is 25C";
	memcpy(&temp_msg.msg,&my_msg,sizeof(my_msg));
	
	struct mq_attr attr;
	attr.mq_maxmsg = 256;
	attr.mq_msgsize = sizeof(Message_t);
	attr.mq_flags = 0;

	mqd_t log_queue_handle1 =  mq_open(LOGGER_QUEUE,O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, NULL);	
	
	if(mq_send(log_queue_handle1, (int8_t*)&temp_msg, sizeof(Message_t), 0) != 0)
	{
		perror("QUEUE SEND ERROR");
        //exit(EXIT_FAILURE); 
	}
	else
	{
		flag = 1; 
	}
	mq_close(log_queue_handle1);
	//pthread_mutex_unlock(&log_queue_mutex);
}

void *lightThread(void *threadArgs)
{
}

void *socketThread(void *threadArgs)
{
}

int main()
{
    int32_t isThreadCreated = 0;
    
    /* Main Task Handling Code will come here */
    //init_queue();

    pthread_mutex_init(&log_queue_mutex,NULL);
	pthread_mutex_init(&temp_queue_mutex,NULL);
	pthread_mutex_init(&main_queue_mutex,NULL);
	pthread_mutex_init(&light_queue_mutex,NULL);

    /* Create Logging Thread */
    isThreadCreated = pthread_create(&threads[LOG_THREAD],NULL,&logThread,NULL);
    if(isThreadCreated != 0)
    {
	printf("Thread Creation failed, error code - %d\n", isThreadCreated);
    }
    
    /* Create Temperature Thread */
    isThreadCreated = pthread_create(&threads[TEMP_THREAD],NULL,&tempThread,NULL);
    if(isThreadCreated != 0)
    {
	printf("Thread Creation failed, error code - %d\n", isThreadCreated);
    }

    /* Create Light Thread 
    isThreadCreated = pthread_create(&threads[LIGHT_THREAD],NULL,&lightThread,NULL);
    if(isThreadCreated != 0)
    {
	printf("Thread Creation failed, error code - %d\n", isThreadCreated);
    }*/

    /* Create Socket Thread 
    isThreadCreated = pthread_create(&threads[SOCKET_THREAD],NULL,&socketThread,NULL);
    if(isThreadCreated != 0)
    {
	printf("Thread Creation failed, error code - %d\n", isThreadCreated);
    }
	*/
	
    /* join the pthread with the existing processes */
    pthread_join(threads[TEMP_THREAD], NULL);
    //pthread_join(threads[LIGHT_THREAD], NULL);
    //pthread_join(threads[SOCKET_THREAD], NULL);
    pthread_join(threads[LOG_THREAD], NULL);

    return 0;
}



