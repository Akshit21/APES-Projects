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
* @author : Akshit Shah, Shuting Guo
* @date : 02/22/2018
*
* @file : led.c
* @brief : GPIO LED APIs
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference :
***************************************************************************************************/

#include "project.h"

/* Function to turn on the led */
Status_t ledOn(char *ledAddr)
{
  FILE *pFile = NULL;

	if((pFile = fopen(ledAddr,"w")) == NULL)
		return ERROR;

  if(fwrite("1",1,1,pFile) < 0)
		return ERROR;

  if(fclose(pFile) != 0)
    return ERROR;

  return SUCCESS;
}

/* Function to turn of the led */
Status_t ledOff(char *ledAddr)
{
  FILE *pFile = NULL;

	if((pFile = fopen(ledAddr,"w")) == NULL)
		return ERROR;

  if(fwrite("0",1,1,pFile) < 0)
		return ERROR;

  if(fclose(pFile) != 0)
        return ERROR;

  return SUCCESS;
}

/* Function to Blink an LED */
void blinkLED(void)
{
  ledOn(ledPath);
  sleep(1);
  ledOff(ledPath);
  sleep(1);
  ledOn(ledPath);
}
