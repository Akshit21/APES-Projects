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
* @file : logThread.c
* @brief : Logger Thread function declaration
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference : 
***************************************************************************************************/

#include "main.h"

FILE *pLogfile;
struct mq_attr attr;

void *logThread(void *threadArgs)
{
    printf("In Logger Thread\n");
    uint32_t logger_data;

    /* Create a messaging structure */
    Message_t *msg;
    
	Status_t status =  create_message_info(&msg);
    if(status != SUCCESS)
		printf("Do Something and stop!!\n");

	//Currently file name hard coded later take it from command line
	status = Status_t get_log_file(&pLogfile, "test.txt")
	if(status != SUCCESS)
		printf("Error Opening the file");
		
	while(1)
	{
		if(1) // @todo SHUTING: Change Condition After you add timer - SOME_EVENT: TIMER OR OTHER SIGNAL
		{
			mq_getattr(log_queue_handle, &attr);
			printf("There are total %ld messages\n", attr.mq_curmsgs);

			/* Process all the data and log it in a file*/
			while(attr.mq_curmsgs > 0)
			{
				pthread_mutex_lock(&log_queue_mutex);
				if(mq_receive(log_queue_handle,(int8_t*)&msg,QUEUE_SIZE + 1,&logger_data) == -1)
				{
					printf("Failed to receive any data for logging\n");
				}
				
				if(msg->requestId == LOG_MSG) /* If logging message */
				{
					printf ("Source ID: %d \n", msg->sourceId);
					printf ("Timestamp: %s \n", msg->timestamp);
					printf ("Log Level: %d \n", msg->type);
					printf ("Message Data: %s \n", logmsg3->payload);
					
					/* Log the data in the file */
					log_data(&pLogfile, msg);
				}
				pthread_mutex_unlock(&log_queue_mutex);
				
				if(1)// @todo SHUTING: Change Condition After you add SIGEVENT
				{
					mq_close(log_queue_handle);
					break;
				}
			}
		}
	}
}
