#include "project.h"

static int32_t accept_client_timeout(int32_t socket_handle, int32_t timeout_s);

void * task_socket(void* param)
{
  Message_t socket_msg = {0};
  ThreadInfo_t info = {0};
	Status_t status = SUCCESS;
  int socketfd, newsocketfd, portno=PORT_NO;
  struct sockaddr_in serv_addr;
  fd_set write_mask;
  struct timeval tv = {.tv_sec=1, .tv_usec=0};

  /* Create a TCP/IP socket */
  if((socketfd = socket(AF_INET, SOCK_STREAM, 0))<0)
  {
    perror("Failed to create a TCP/IP socket.\n");
    status = ERROR;
  }

  /* Clear address structure */
  bzero((char *)&serv_addr, sizeof(serv_addr));
  /* Setup the host_addr structre for use in blind call */
  serv_addr.sin_family = AF_INET;
  /* Automatically be filled with current host's IP address */
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  /* Bind to the port */
  serv_addr.sin_port = htons(portno);
  
  /* Bind the socket to the current IP address on port */
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
	  DEBUG("[DEBUG] SOCKET task running.\n");
    /* Check if any client request */
	//if(newsocketfd = accept4(socketfd, NULL, NULL, SOCK_NONBLOCK))
    if((newsocketfd=accept_client_timeout(socketfd, 2))!=0)
    {
	    DEBUG("[DEBUG] SOCKET task received request from clients.\n");
      if(read(newsocketfd, &socket_msg, sizeof(socket_msg)) == sizeof(socket_msg))
      {
        /* Send out request to corresponding tasks */
        switch (socket_msg.requestId)
        {
          case GET_TEMP_C:
          case GET_TEMP_K:
          case GET_TEMP_F:
            socket_msg = create_message_struct(SOCKET_THREAD, TEMPTHREAD, INFO, socket_msg.requestId);
            info.data = socket_msg;
            info.thread_mutex_lock = temp_queue_mutex;
            info.qName = TEMP_QUEUE;
            msg_send(&info);
            DEBUG("[DEBUG] SOCKET task sent out temperature request to TMP task.\n");
            /*LOG THE REQUEST*/
            memset(info.data.msg, 0, sizeof(info.data.msg));
            sprintf(info.data.msg, "Request from client to get Temp");
            info.data.destId = LOGGERTHREAD;
            info.thread_mutex_lock = log_queue_mutex;
            info.qName = LOGGER_QUEUE;
            msg_send(&info);
            DEBUG("[DEBUG] SOCKET task sent out LOG_MSG request to LOGGER task.\n");
            break;
          case GET_LIGHT:
          case GET_LIGHT_STATE:
            socket_msg = create_message_struct(SOCKET_THREAD, LIGHTTHREAD, INFO, socket_msg.requestId);
            info.data = socket_msg;
            memset(info.data.msg, 0, sizeof(info.data.msg));
            info.thread_mutex_lock = light_queue_mutex;
            info.qName = LIGHT_QUEUE;
            msg_send(&info);
            DEBUG("[DEBUG] SOCKET task sent out light request to LIGHT task.\n");
            /*LOG THE REQUEST*/
            memset(info.data.msg, 0, sizeof(info.data.msg));
            sprintf(info.data.msg, "Request from Client to get Light");
            info.data.destId = LOGGERTHREAD;
            info.thread_mutex_lock = log_queue_mutex;
            info.qName = LOGGER_QUEUE;
            msg_send(&info);
            DEBUG("[DEBUG] SOCKET task sent out LOG_MSG request to LOGGER task.\n");
            break;
          default:;
        }
      }
      else
      {
        DEBUG("[DEBUG] SCOKET task Failed to read incoming packet.\n");
        close(newsocketfd);
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
            DEBUG("[DEBUG] SOCKET task responeded to HEARTBEAT request.\n");
	    break;
          case GET_TEMP_C:
          case GET_TEMP_F:
          case GET_TEMP_K:
          case GET_LIGHT:
          case GET_LIGHT_STATE:
	    FD_ZERO(&write_mask);
	    FD_SET(newsocketfd, &write_mask);
            //if(select(newsocketfd, (fd_set*)0, &write_mask, (fd_set*)0, &tv)>0)
	    //{
	    	//DEBUG("[DEBUG] SOCKET task sending out...\n");
		send(newsocketfd, &socket_msg, sizeof(socket_msg), 0);
	    //}
	    //write(newsocketfd, &socket_msg, sizeof(socket_msg));
	    close(newsocketfd);
            DEBUG("[DEBUG] SOCKET task responded to client request.********************\n");
            break;
          default:
            DEBUG("[DEBUG] SOCKET task received unrecognized request.\n");
        }
      }
    }

  }
  DEBUG("[DEBUG] SOCKET task exits.\n");
  pthread_mutex_destroy(&socket_queue_mutex);
  mq_unlink(SOCKET_QUEUE);
  /* Blink LED */
  blinkLED();
  pthread_exit(NULL);
}

static int32_t accept_client_timeout(int32_t socket_handle, int32_t timeout_s)
{
	int32_t ret;
	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(socket_handle, &rfds);

	tv.tv_sec = (long)timeout_s;
	tv.tv_usec = 0;
	
	if((ret=select(socket_handle+1, &rfds, (fd_set *)0, (fd_set*)0, &tv))>0)
	{
			return accept(socket_handle, NULL, NULL);
	}
	else
		return 0;
}

// void client_request_process(void * param)
// {
//   Message_t socket_msg;
//   ThreadInfo_t info;
//   servicefd = *(int*)param;
//   pthread_detach(pthread_self());
//   if(read(servicefd, &socket_msg, sizeof(socket_msg)) == sizeof(socket_msg))
//   {
//     /* Send out request to corresponding tasks */
//     switch (packet.msg.requestId)
//     {
//       case GET_TEMP_C:
//       case GET_TEMP_K:
//       case GET_TEMP_F:
//         socket_msg = create_message_struct(SOCKET_THREAD, TEMPTHREAD, INFO, socket_msg.requestId);
//         info.data = socket_msg;
//         info.thread_mutex_lock = temp_queue_mutex;
//         info.qName = TEMP_QUEUE;
//         msg_send(&info);
//         /*LOG THE REQUEST*/
//         memset(info.data.msg, 0, sizeof(info.data.msg));
//         sprintf(info.data.msg, "Request from Client to get Temp");
//         info.data.destId = LOGGERTHREAD;
//         info.thread_mutex_lock = log_queue_mutex;
//         info.qName = LOGGER_QUEUE;
//         msg_send(&info);
//         break;
//       case GET_LIGHT:
//       case GET_LIGHT_STATE:
//         socket_msg = create_message_struct(SOCKET_THREAD, LIGHTTHREAD, INFO, socket_msg.requestId);
//         info.data = socket_msg;
//         memset(info.data.msg, 0, sizeof(info.data.msg));
//         info.thread_mutex_lock = light_queue_mutex;
//         info.qName = LIGHT_QUEUE;
//         msg_send(&info);
//         /*LOG THE REQUEST*/
//         memset(info.data.msg, 0, sizeof(info.data.msg));
//         sprintf(info.data.msg, "Request from Client to get Light");
//         info.data.destId = LOGGERTHREAD;
//         info.thread_mutex_lock = log_queue_mutex;
//         info.qName = LOGGER_QUEUE;
//         msg_send(&info);
//         break;
//       default:;
//     }
//   }
//   else
//   {
//     DEBUG("[DEBUG] SCOKET task Failed to read incoming packet.\n");
//     close(servicefd);
//   }
//
//   /* Send out response to the requesting client */

//}
