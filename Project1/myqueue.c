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

void init_queue()
{
	struct mq_attr attr;
	attr.mq_maxmsg = 20;
	attr.mq_msgsize = QUEUE_SIZE;
	attr.mq_flags = 0;

	log_queue_handle =  mq_open(LOGGER_QUEUE,O_RDWR | O_CREAT,0666,&attr);
	temp_queue_handle =  mq_open(TEMP_QUEUE,O_RDWR | O_CREAT,0666,&attr);
	light_queue_handle =  mq_open(LIGHT_QUEUE,O_RDWR | O_CREAT,0666,&attr);
	main_queue_handle =  mq_open(MAIN_QUEUE,O_RDWR | O_CREAT,0666,&attr);
	socket_queue_handle =  mq_open(SOCKET_QUEUE,O_RDWR | O_CREAT,0666,&attr);

	if (log_queue_handle == -1 || temp_queue_handle == -1 ||
		light_queue_handle == -1 || main_queue_handle == -1 ||
		socket_queue_handle == -1)
	{
		printf("Error Opening Queue\n");
	}
	else
	{
		printf("Queue Created successfully\n");
	}
}


