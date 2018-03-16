#include "project.h"


int main(int argc, char const *argv[])
{
  pthread_t thread1, thread2, thread3, thread4;
  int32_t tmp_handle, apds_handle;

  /* Connect the sensor */
  tmp1021_init(&tmp_handle);
  apds9301_init(&apds_handle);

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

#ifdef TEMP_TASK
  pthread_join(thread1, NULL);
#endif
#ifdef LIGHT_TASK
  pthread_join(thread2, NULL);
#endif
#ifdef SOCKET_TASK
  pthread_join(thread3, NULL);
#endif
  exit(EXIT_SUCCESS);
}
