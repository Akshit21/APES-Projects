#include "project.h"


int main(int argc, char const *argv[])
{
  pthread_t thread1, thread2, thread3, thread4;
  int32_t tmp_handle, apds_handle;


  /* Connect the sensor */
#ifdef TEMP_TASK
  if(tmp1021_init(&tmp_handle)==-1)
  	printf("tmp init failed\n");
#endif

#ifdef LIGHT_TASK
  apds9301_init(&apds_handle);
#endif

#ifdef SENSOR_STARTUP_TEST
  /* Sensor startup test */

#endif

  /* Create task threads */
#ifdef TEMP_TASK
  if(pthread_create(&thread1, NULL, task_tmp, (void *)&tmp_handle))
  {
    perror("Failed to create the temperature task.");
    exit(EXIT_FAILURE);
  }
#endif
#ifdef LIGHT_TASK
  if(pthread_create(&thread2, NULL, task_light, (void *)&apds_handle))
  {
    perror("Failed to create the light task.");
    exit(EXIT_FAILURE);
  }
#endif

#ifdef SOCKET_TASK
  if(pthread_create(&thread3, NULL, task_socket, NULL))
  {
    perror("Failed to create the socket task.");
    exit(EXIT_FAILURE);
  }
#endif
#ifdef LOGGER_TASK
  if(pthread_create(&thread4, NULL, task_log, NULL))
  {
    perror("Failed to create the socket task.");
    exit(EXIT_FAILURE);
  }
#endif
#ifdef MAIN_HEARTBEAT_REQUEST
  /* Heartbeat processing */
  //request_heartbeat();
  Message_t main_msg = {0};
  ThreadInfo_t info = {0};
  while(1)
  {
    request_heartbeat();
    while(main_queue_flag--)
    {
      memset(info.data.msg, 0, sizeof(info.data.msg));
      info.thread_mutex_lock = main_queue_mutex;
      info.qName = MAIN_QUEUE;
      msg_receive(&info);
      main_msg = info.data;
      if (main_msg.requestId == HEART_BEAT)
      {
        printf ("Source ID: %d \n", main_msg.sourceId);
        printf ("Timestamp: %s", ctime(&main_msg.timeStamp));
        printf ("Log Level: %s \n", levels[main_msg.type]);
        printf ("Message Data: %s \n", main_msg.msg);
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
      }
    }
    sleep(5);
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
