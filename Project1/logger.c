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
* @file : logger.c
* @brief : Data logging Helper Functions
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference : 
***************************************************************************************************/

#include "main.h"

/* Create a Messaging Structure */
Status_t create_message_info(Message_t **message)
{
    if(*message = (Message_t *)malloc(sizeof(Message_t)) == NULL)
    {
        printf("MALLOC ERROR\n");
    	return NULL_PTR;
	}

	memset(*message,(uint8_t)`\0',sizeof(Message_t));
	return SUCCESS;
}


Status_t log_data(FILE **pfile, Message_t *message)
{
	if(*pfile == NULL)	return NULL_PTR;
	if(message == NULL)	return NULL_PTR;

	char logData[100] = {(uint8_t)'\0'};
	sprintf(logData, "Task: %s\tTime: %s\n%s: %s\n\n",
		task[message->sourceId], ctime(&message->timeStamp), 
		level[message->type], message->msg) 

	if(fwrite(logData, strlen(logData), 1, pFile) < 0)
	{
		printf("Error in Writing Data");
		return ERROR_WRITE;
	}

	return SUCCESS;
}

Status_t get_log_file(FILE **pfile, char *fileName)
{
	if((*pFile=fopen(fileName,"w+")) == NULL)
	{
		printf("Error in Creating File\n");
		return ERROR_OPEN;
	}
	return SUCCESS;
}
