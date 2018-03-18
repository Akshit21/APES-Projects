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

extern int log_flag, temp_flag, light_flag, socket_flag, main_flag;
extern char *levels[];

void init_queue()
{

	mq_unlink (LOGGER_QUEUE);
	mq_unlink (TEMP_QUEUE);
	mq_unlink (LIGHT_QUEUE);

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
		//exit(EXIT_FAILURE);
	}
	else
	{
		printf("Queue Created successfully\n");
	}
}


void *logThread(void *threadArgs)
{
    printf("In Logger Thread\n");
    Message_t pMsg = {0};
    ThreadInfo_t info = {0};
	info.data = pMsg;
	char msg1[20] = {(uint8_t)'\0'};;

	FILE *pfile;
	char *fileName = "logFile.txt";
	get_log_file(&pfile, fileName);

	while (1)
	{
		while (log_flag--)
		{

			info.thread_mutex_lock = log_queue_mutex;
			info.qName = LOGGER_QUEUE;
			msg_receive(&info);

			pMsg = info.data;
			switch(pMsg.requestId)
			{
				case LOG_MSG:
					printf ("Source ID: %d \n", pMsg.sourceId);
					printf ("Timestamp: %s", ctime(&pMsg.timeStamp));
					printf ("Log Level: %s \n", levels[pMsg.type]);
					printf ("Message Data: %s \n", pMsg.msg);
					log_data(&pfile, &pMsg, fileName);
					break;
				case HEART_BEAT:
					sprintf(msg1,"Log Thread is Alive");
					create_message_struct(&pMsg, LOGGER_THREAD,
							MAINTHREAD, HEARTBEAT,
							HEART_BEAT, msg1);
					info.data = pMsg;
					info.thread_mutex_lock = main_queue_mutex;
					info.qName = MAIN_QUEUE;
					msg_send(&info);
					printf("Logger sent to the msg Main\n");
					break;
				case SHUT_DOWN:
					break; //EXIT CODE
				default:
					break;
			}
		}
		//sleep(1);
	}
}

void *tempThread(void *threadArgs)
{
	printf("In Temp Thread\n");

	ThreadInfo_t info;
	Message_t temp_msg;

	float temp_val = 20.5; // will be removed when integrated
	char msg[20] = {(uint8_t)'\0'};
	char msg1[20] = {(uint8_t)'\0'};
	/* floating point value to ascii */
	sprintf(msg,"Temp Value: %0.3fC",temp_val);
	create_message_struct(&temp_msg, TEMP_THREAD,
						  LOGGERTHREAD, INFO,
						  LOG_MSG, msg);

	while(1)
	{
		info.data = temp_msg;
		info.thread_mutex_lock = log_queue_mutex;
		info.qName = LOGGER_QUEUE;
		msg_send(&info);

		//sleep(1);
		while (temp_flag--)
		{
			printf("Temp Flag:\n");

			//info.data = {0};
			info.thread_mutex_lock = temp_queue_mutex;
			info.qName = TEMP_QUEUE;
			msg_receive(&info);

			temp_msg = info.data;
			switch(temp_msg.requestId)
			{
				case HEART_BEAT:
					sprintf(msg1,"Temp Thread is Alive");
					create_message_struct(&temp_msg, TEMP_THREAD,
							MAINTHREAD, HEARTBEAT,
							HEART_BEAT, msg1);
					info.data = temp_msg;
					info.thread_mutex_lock = main_queue_mutex;
					info.qName = MAIN_QUEUE;
					msg_send(&info);
					break;
				case SHUT_DOWN:
					break; //EXIT CODE
				case GET_TEMP:
					// Get the temperature value for external request
					create_message_struct(&temp_msg, TEMP_THREAD,
							SOCKETTHREAD, INFO,
							GET_TEMP, msg);
					info.data = temp_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					break;
				default:
					break;
			}
		}
    }
}

void *lightThread(void *threadArgs)
{
	printf("In Light Thread\n");

	ThreadInfo_t info;
	Message_t light_msg;

	float light_val = 20.5; // will be removed when integrated
	char msg[20] = {(uint8_t)'\0'};
	char msg1[20] = {(uint8_t)'\0'};
	/* floating point value to ascii */
	sprintf(msg,"Light Value: %0.3f",light_val);
	create_message_struct(&light_msg, LIGHT_THREAD,
						  LOGGERTHREAD, INFO,
						  LOG_MSG, msg);

	while(1)
	{
		info.data = light_msg;
		info.thread_mutex_lock = log_queue_mutex;
		info.qName = LOGGER_QUEUE;
		msg_send(&info);

		//sleep(1);
		while (light_flag--)
		{
			printf("Light Flag:\n");
			//info.data = {0};
			info.thread_mutex_lock = light_queue_mutex;
			info.qName = LIGHT_QUEUE;
			msg_receive(&info);

			light_msg = info.data;
			switch(light_msg.requestId)
			{
				case HEART_BEAT:
					sprintf(msg1,"Light Thread is Alive");
					create_message_struct(&light_msg, LIGHT_THREAD,
							MAINTHREAD, HEARTBEAT,
							HEART_BEAT, msg1);
					info.data = light_msg;
					info.thread_mutex_lock = main_queue_mutex;
					info.qName = MAIN_QUEUE;
					msg_send(&info);
					break;
				case SHUT_DOWN:
					break; //EXIT CODE
				case GET_LIGHT:
					// Get the light value for external request
					create_message_struct(&light_msg, LIGHT_THREAD,
							SOCKETTHREAD, INFO,
							GET_LIGHT, msg);
					info.data = light_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					break;
				case GET_LIGHT_STATE:
					// Get the light state for external request
					create_message_struct(&light_msg, LIGHT_THREAD,
							SOCKETTHREAD, INFO,
							GET_LIGHT, msg);
					info.data = light_msg;
					info.thread_mutex_lock = socket_queue_mutex;
					info.qName = SOCKET_QUEUE;
					msg_send(&info);
					break;
				default:
					break;
			}
		}
    }
}

void *socketThread(void *threadArgs)
{

}

int main()
{
    int32_t isThreadCreated = 0;

    /* Main Task Handling Code will come here */
    init_queue();

    pthread_mutex_init(&log_queue_mutex,NULL);
	pthread_mutex_init(&temp_queue_mutex,NULL);
	pthread_mutex_init(&main_queue_mutex,NULL);
	pthread_mutex_init(&light_queue_mutex,NULL);
	pthread_mutex_init(&socket_queue_mutex,NULL);

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

    /* Create Light Thread */
    isThreadCreated = pthread_create(&threads[LIGHT_THREAD],NULL,&lightThread,NULL);
    if(isThreadCreated != 0)
    {
	printf("Thread Creation failed, error code - %d\n", isThreadCreated);
    }

    /* Create Socket Thread */
    isThreadCreated = pthread_create(&threads[SOCKET_THREAD],NULL,&socketThread,NULL);
    if(isThreadCreated != 0)
    {
	printf("Thread Creation failed, error code - %d\n", isThreadCreated);
    }

    /* HEART BEAT */
    //request_heartbeat();
    Message_t main_msg = {0};
    ThreadInfo_t info = {0};
		info.data = main_msg;
		while(1)
		{
			request_heartbeat();
    	while(main_flag--)
    	{
				printf("Main Flag:\n");
				memset(info.data.msg, 0, sizeof(info.data.msg));
    		info.thread_mutex_lock = main_queue_mutex;
				info.qName = MAIN_QUEUE;
				msg_receive(&info);

				main_msg = info.data;
				printf("Request PID:%d\n",main_msg.requestId);
				if (main_msg.requestId == HEART_BEAT)
				{
					FILE *pfile;
					char *fileName = "logFile.txt";
					printf ("Source ID: %d \n", main_msg.sourceId);
					printf ("Timestamp: %s", ctime(&main_msg.timeStamp));
					printf ("Log Level: %s \n", levels[main_msg.type]);
					printf ("Message Data: %s \n", main_msg.msg);
					//log_data(&pfile, &main_msg, fileName);
					switch(main_msg.sourceId)
					{
						case 1:
							break;
						case 2:
							break;
						case 3:
							break;
						case 4:
							break;
						default:;
					}
					printf("It is here\n");
				}
    	}
			sleep(5);
    }
    /* join the pthread with the existing processes */
    pthread_join(threads[TEMP_THREAD], NULL);
    pthread_join(threads[LIGHT_THREAD], NULL);
    pthread_join(threads[SOCKET_THREAD], NULL);
    pthread_join(threads[LOG_THREAD], NULL);

    return 0;
}
