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

void *logThread(void *threadArgs)
{
    printf("In Logger Thread\n");

    /* Create a messaging structure */
    Message_t *msg = NULL;
    Status_t status =  create_message_info(&msg);
    if(status != SUCCESS)
	printf("Do Something and stop!!\n");
    
    // Else do your thread execution 
}
