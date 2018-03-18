/***************************************************************************************************
* Copyright (C) 2017 by Akshit Shah, Shuting Guo
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Akshit Shah, Shuting Guo, Prof Alex Fosdick and the University of Colorado are
* not liable for any misuse of this material.
***************************************************************************************************/
/***************************************************************************************************
* @author : Akshit Shah, Shuting Guo
* @date : 03/05/2018
*
* @file : task.h
* @brief : This header file provides an abstaction of all thread routines
***************************************************************************************************/
#ifndef _TASK_H__
#define _TASK_H__

#include "project.h"

#define TMP_PERIOD	(2000)
#define RETRY_MAX   (3)

#define PORT_NO (9999)

void * task_tmp(void * param);
void * task_light(void * param);
void * task_socket(void * param);
void * task_log(void * param);

Status_t tmp1021_init(int32_t *dev_fp);
Status_t apds9301_init(int32_t *dev_fp);



#endif
