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
* @date : 03/10/2018
*
* @file : main.c
* @brief : Main thread handling
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference :
***************************************************************************************************/

#include "project.h"

int main(int argc, char const *argv[])
{
  pthread_t thread1, thread2, thread3, thread4;
  int32_t tmp_handle, apds_handle;
  Status_t status = SUCCESS;

  /* Connect the sensor */
#ifdef TEMP_TASK
  if((status = tmp1021_init(&tmp_handle))==ERROR)
  {
    DEBUG("[DEBUG] Failed to initialize the tmp1021 sensor.\n");
  }
#endif

#ifdef LIGHT_TASK
  if((status = apds9301_init(&apds_handle))==ERROR)
  {
    DEBUG("[DEBUF] Failed to initialize the apds9301 sensor.\n");
  }
#endif

#ifdef SENSOR_STARTUP_TEST
  /* Sensor startup test */
  if(status == ERROR)
  {
    DEBUG("[DEBUG] Sensor start up test failed, exit.\n");
    //exit(EXIT_FAILURE);
  }
#endif

  /* Create task threads */
#ifdef TEMP_TASK
  if(pthread_create(&thread1, NULL, task_tmp, (void *)&tmp_handle))
  {
    perror("Failed to create the temperature task.");
    //exit(EXIT_FAILURE);
  }
#endif
#ifdef LIGHT_TASK
  if(pthread_create(&thread2, NULL, task_light, (void *)&apds_handle))
  {
    perror("Failed to create the light task.");
    //exit(EXIT_FAILURE);
  }
#endif

#ifdef SOCKET_TASK
  if(pthread_create(&thread3, NULL, task_socket, NULL))
  {
    perror("Failed to create the socket task.");
    //exit(EXIT_FAILURE);
  }
#endif
#ifdef LOGGER_TASK
  if(pthread_create(&thread4, NULL, task_log, NULL))
  {
    perror("Failed to create the socket task.");
    //exit(EXIT_FAILURE);
  }
#endif

/* Startup Test Failed */
if (status == ERROR)
{
  /* Blink LED */
  blinkLED();
  exit(EXIT_FAILURE);
}
/* Startup test sucessful */
Message_t main_msg = {0};
ThreadInfo_t info = {0};

main_msg = create_message_struct(MAIN_THREAD, LOGGERTHREAD, INIT, LOG_MSG);
sprintf(main_msg.msg,"STARTUP COMPLETE SUCCESSFULLY");
info.data = main_msg;
info.thread_mutex_lock = log_queue_mutex;
info.qName = LOGGER_QUEUE;
msg_send(&info);

#ifdef MAIN_HEARTBEAT_REQUEST
  /* Heartbeat processing */
  uint32_t chance = 0;
  uint32_t heartbeat_monitor[4] = {0};
  while(1)
  {
    /* Send heart beat request */
    request_heartbeat();
    chance++;
    /* Process the heart beat response */
    while(main_queue_flag)
    {
	    main_queue_flag--;
      memset(info.data.msg, 0, sizeof(info.data.msg));
      info.thread_mutex_lock = main_queue_mutex;
      info.qName = MAIN_QUEUE;
      /* Receive HEARTBEAT response */
      msg_receive(&info);
      main_msg = info.data;
      if (main_msg.requestId == HEART_BEAT)
      {
        DEBUG ("[DEBUG] Source ID: %d \n", main_msg.sourceId);
        DEBUG ("[DEBUG] Timestamp: %s", ctime(&main_msg.timeStamp));
        DEBUG ("[DEBUG] Log Level: %s \n", levels[main_msg.type]);
        DEBUG ("[DEBUG] Message Data: %s \n", main_msg.msg);
        heartbeat_monitor[main_msg.sourceId - 1]++;
      }
    }
    sleep(10);
    if(chance == 3)
    {
      chance = 0;
      for(int i=4; i>0; i--)
      {
        if(heartbeat_monitor[i-1] == 0)
        {
		DEBUG("[DEBUG] id: %d thread did not respond. Call Cleanup.\n", i-1);
          /*Try logging if Logger Thread is active*/
          if(heartbeat_monitor[0])
          {
            memset(&info,0,sizeof(info));
            info.data = create_message_struct(MAIN_THREAD,LOGGERTHREAD,ERROR_MSG,LOG_MSG);
            sprintf(info.data.msg,"SHUTTING DOWN THE SYSTEM: %s Thread Inactive", task[i+1]);
            info.thread_mutex_lock = log_queue_mutex;
            info.qName = LOGGER_QUEUE;
            msg_send(&info);
            sleep(2);/*Wait for logger thread to process*/
            /* Blink LED */
            blinkLED();
          }
       
        }
        heartbeat_monitor[i-1] = 0;
      }
    }
  }
#endif
  /* Wait for child threads */
#ifdef TEMP_TASK
  pthread_join(thread1, NULL);
#endif
#ifdef LIGHT_TASK
  pthread_join(thread2, NULL);
#endif
#ifdef SOCKET_TASK
  pthread_join(thread3, NULL);
#endif
#ifdef LOGGER_TASK
  pthread_join(thread4, NULL);
#endif
  exit(EXIT_SUCCESS);
}
