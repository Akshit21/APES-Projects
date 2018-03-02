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
* @file : main.c
* @brief : Entry Point of the Program. Thread creation and initiailization
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference : 
***************************************************************************************************/

#include "main.h"

int main()
{
    int32_t isThreadCreated = 0;

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

    /* Main Task Handling Code will come here */

    /* join the pthread with the existing processes */
    pthread_join(threads[LOG_THREAD], NULL);
    pthread_join(threads[TEMP_THREAD], NULL);
    pthread_join(threads[LIGHT_THREAD], NULL);
    pthread_join(threads[SOCKET_THREAD], NULL);

    return 0;
}
