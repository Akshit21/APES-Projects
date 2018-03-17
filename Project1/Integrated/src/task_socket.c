#include "project.h"


void * task_socket(void* param)
{

  Message_t socket_msg = {0};
  ThreadInfo_t info = {0};
	Status_t status = SUCCESS;
  int socketfd, newsocketfd, portno=PORT_NO;
  /* Create a TCP/IP socket */
  if(socketfd = socket(AF_INET, SOCK_STREAM, 0))<0)
  {
    perror("Failed to create a TCP/IP socket.\n");
    status = ERROR;
  }
  struct sockaddr_in serv_addr;
  // Clear address structure */
  bzero((char *)&serv_addr, sizeof(serv_addr));
  /* Setup the host_addr structre for use in blind call */
  serv_addr.sin_family = AF_INET;
  /* Automatically be filled with current host's IP address */
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  /* Bind to the port */
  serv_addr.sin_port = htons(portno);
  /* BInd the socket to the current IP address on port */
  if(bind(socketfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0)
  {
    perror("Failed to bind socket to the current IP address on port.\n");
    status = ERROR;
  }
  /* Listen to incoming connections
     The listen() function call places all incoming connection into a backlog queue
     until accept() call accepts the connection.
     The maximum size for the backlog queue is set to 5 */
  listen(socketfd, 10);
  while(status == SUCCESS)
  {
    if(newsocketfd = accept(socketfd, NULL, NULL))
    {
      pthread_t one_time_task;
      if(pthread_create(&one_time_task, 0, client_request_process, (void*)&newsocketfd))
      {
        DEBUG("[DEBUG] Failed to pawn the thread for processing client request.\n");
        status = ERROR;
      }
    }

    /*Process heartbeat request */
    while(socket_queue_flag)
    {
      socket_queue_flag--;
      memset(&info.data, 0, sizeof(info.data));
      info.thread_mutex_lock = socket_queue_mutex;
      info.qName = SOCKET_QUEUE;
      if((status = msg_receive(&info))==SUCCESS)
      {
        socket_msg = info.data;
        switch(socket_msg.requestId)
        {
          case HEART_BEAT:
		        DEBUG("[DEBUG] SOCKET task received HEARTBEAT request.\n");
            socket_msg = create_message_struct(SOCKET_THREAD, MAINTHREAD, HEARTBEAT, HEART_BEAT);
            info.data = socket_msg;
            info.thread_mutex_lock = main_queue_mutex;
            info.qName = MAIN_QUEUE;
            status = msg_send(&info);
            break;
          default:
            DEBUG("[DEBUG] SOCKET task received unrecognized request.\n");
        }
      }
    }
  }
  DEBUG("[DEBUG] SOCKET task exits.\n");
  /* Exit success */
  exit(EXIT_SUCCESS);
}

void client_request_process(void * param)
{
  Message_t socket_msg;
  ThreadInfo_t info;
  servicefd = *(int*)param;
  pthread_detach(pthread_self());
  if(read(servicefd, &socket_msg, sizeof(socket_msg)) == sizeof(socket_msg))
  {
    /* Send out request to corresponding tasks */
    switch (packet.msg.requestId)
    {
      case GET_TEMP_C:
      case GET_TEMP_K:
      case GET_TEMP_F:
        socket_msg = create_message_struct(SOCKET_THREAD, TEMPTHREAD, INFO, socket_msg.requestId);
        info.data = socket_msg;
        info.thread_mutex_lock = temp_queue_mutex;
        info.qName = TEMP_QUEUE;
        msg_send(&info);
        /*LOG THE REQUEST*/
        memset(info.data.msg, 0, sizeof(info.data.msg));
        sprintf(info.data.msg, "Request from Client to get Temp");
        info.data.destId = LOGGERTHREAD;
        info.thread_mutex_lock = log_queue_mutex;
        info.qName = LOGGER_QUEUE;
        msg_send(&info);
        break;
      case GET_LIGHT:
      case GET_LIGHT_STATE:
        socket_msg = create_message_struct(SOCKET_THREAD, LIGHTTHREAD, INFO, socket_msg.requestId);
        info.data = socket_msg;
        memset(info.data.msg, 0, sizeof(info.data.msg));
        info.thread_mutex_lock = light_queue_mutex;
        info.qName = LIGHT_QUEUE;
        msg_send(&info);
        /*LOG THE REQUEST*/
        memset(info.data.msg, 0, sizeof(info.data.msg));
        sprintf(info.data.msg, "Request from Client to get Light");
        info.data.destId = LOGGERTHREAD;
        info.thread_mutex_lock = log_queue_mutex;
        info.qName = LOGGER_QUEUE;
        msg_send(&info);
        break;
      default:;
    }
  }
  else
  {
    DEBUG("[DEBUG] SCOKET task Failed to read incoming packet.\n");
    close(servicefd);
  }

  /* Send out response to the requesting client */

}
