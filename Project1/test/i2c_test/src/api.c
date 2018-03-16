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
* @date : 03/1/2018
*
* @file : logger.c
* @brief : Helper Functions
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference :
***************************************************************************************************/

#include "project.h"

char *task[] = {"MAIN", "LOG DATA", "TEMP SENSE", "LIGHT SENSE", "SOCKET"};
char *levels[] = {"INFO", "WARNING", "ERROR", "HEARTBEAT", "INIT"};

int log_flag = 0, temp_flag = 0;
int light_flag = 0, socket_flag = 0;
int main_flag = 0;

Status_t log_data(FILE **pfile, Message_t *message, char *fileName)
{
	if(*pfile == NULL)	return NULL_PTR;
	if(message == NULL)	return NULL_PTR;

	if((*pfile=fopen(fileName,"a+")) == NULL)
	{
		printf("Error in Creating/Opening File\n");
		return ERROR;
	}

	char logData[100] = {(uint8_t)'\0'};
	sprintf(logData, "Task: %s\tTime: %s\n%s: %s\n\n",
		task[message->sourceId], ctime(&message->timeStamp),
		levels[message->type], message->msg);

	if(fwrite(logData, strlen(logData), 1, *pfile) < 0)
	{
		printf("Error in Writing Data");
		return ERROR;
	}

	fclose(*pfile);
	return SUCCESS;
}

Status_t get_log_file(FILE **pfile, char *fileName)
{
	if((*pfile=fopen(fileName,"w")) == NULL)
	{
		printf("Error in Creating File\n");
		return ERROR;
	}
	//fclose(pfile);
	return SUCCESS;
}

Message_t create_message_struct(Source_t src, Dest_t dest, LogLevel_t level,
							RequestId_t req)
{
	Message_t pMsg;
	
	pMsg->sourceId = src;
	pMsg->destId = dest;
	pMsg->type = level;
	pMsg->requestId = req;
	pMsg->timeStamp = time(NULL);
	memcpy(pMsg->msg,msg,strlen(msg));
	memset(pMsg->msg, 0, sizeof(pMsg->msg));
	
	return pMsg
}

void update_flag(Dest_t dest)
{
	switch(dest)
	{
		case MAINTHREAD:
			main_flag++;
			break;
		case LOGGERTHREAD:
			log_flag++;
			break;
		case TEMPTHREAD:
			temp_flag++;
			break;
		case LIGHTTHREAD:
			light_flag++;
			break;
		case SOCKETTHREAD:
			socket_flag++;
			break;
		default:
			break;
	}
}

Status_t msg_send(ThreadInfo_t *info)
{
	struct mq_attr attr;
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = sizeof(Message_t);
	attr.mq_flags = 0;

	pthread_mutex_lock(&info->thread_mutex_lock);
	mqd_t queue_handle =  mq_open(info->qName, O_RDWR, S_IWUSR | S_IRUSR, &attr);
	if (queue_handle == (mqd_t)-1)
		perror("Couldn't create the queue");

	if(mq_send(queue_handle, (char*)&info->data, sizeof(info->data), 0) == -1)
	{
		perror("QUEUE SEND ERROR");
		return ERROR;
		//exit(EXIT_FAILURE);
	}
	mq_close(queue_handle);
	update_flag(info->data.destId);
	pthread_mutex_unlock(&info->thread_mutex_lock);
	sleep(1);
	return SUCCESS;
}

Status_t msg_receive(ThreadInfo_t *info)
{
	struct mq_attr attr;
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = sizeof(Message_t);
	attr.mq_flags = 0;

	pthread_mutex_lock(&info->thread_mutex_lock);
	mqd_t queue_handle =  mq_open(info->qName, O_RDWR, S_IWUSR | S_IRUSR, &attr);
	if (queue_handle == (mqd_t)-1)
		perror("Couldn't create the queue");

	if(mq_receive(queue_handle, (char*)&info->data, 1024, 0) == -1)
	{
		perror("QUEUE SEND ERROR");
		return ERROR;
		//exit(EXIT_FAILURE);
	}
	mq_close(queue_handle);
	pthread_mutex_unlock(&info->thread_mutex_lock);
	sleep(1);
	return SUCCESS;
}

Status_t msg_log(ThreadInfo_t *info)
{
	return msg_send(info);
}

Status_t request_heartbeat()
{
	/*Populate the message structure*/
	Message_t heartbeat_msg = {0};
	char *msg = "Are you Alive?";
	create_message_struct(&heartbeat_msg, MAIN_THREAD,
						  MAINTHREAD, HEARTBEAT,
						  HEART_BEAT, msg);

	heartbeat_msg.destId = LOGGERTHREAD;
	/*Populate Thread Info Structure*/
	ThreadInfo_t info = {0};
	info.data = heartbeat_msg;
	info.thread_mutex_lock = log_queue_mutex;
	info.qName = LOGGER_QUEUE;
	if (msg_send(&info) == ERROR)
		return ERROR;

	heartbeat_msg.destId = TEMPTHREAD;
	/*Populate Thread Info Structure*/
	info.data = heartbeat_msg;
	info.thread_mutex_lock = temp_queue_mutex;
	info.qName = TEMP_QUEUE;
	if (msg_send(&info) == ERROR)
		return ERROR;

	heartbeat_msg.destId = LIGHTTHREAD;
	/*Populate Thread Info Structure*/
	info.data = heartbeat_msg;
	info.thread_mutex_lock = light_queue_mutex;
	info.qName = LIGHT_QUEUE;
	if (msg_send(&info) == ERROR)
		return ERROR;

	heartbeat_msg.destId = SOCKETTHREAD;
	/*Populate Thread Info Structure*/
	info.data = heartbeat_msg;
	info.thread_mutex_lock = socket_queue_mutex;
	info.qName = SOCKET_QUEUE;
	if (msg_send(&info) == ERROR)
		return ERROR;
}
