#include "project.h"

void *task_log(void *param)
{
  Message_t log_msg;
  ThreadInfo_t info;
  Status_t = status = SUCCESS;
	FILE *pfile;
  const char *fileName = "logFile.txt";

  /* Open the file to log */
  if((pfile=fopen(fileName,"w")) == NULL)
	{
    status = ERROR;
	}

	while (status == SUCCESS)
	{
		while (log_queue_flag--)
		{
      memzero(info.data.msg, sizeof(info.data.msg));
			info.thread_mutex_lock = log_queue_mutex;
			info.qName = LOGGER_QUEUE;
			if((status = msg_receive(&info)==SUCCESS);
			{
        log_msg = info.data;
			  switch(log_msg.requestId)
			  {
				  case LOG_MSG:
					  printf ("Source ID: %d \n", pMsg.sourceId);
					  printf ("Timestamp: %s", ctime(&pMsg.timeStamp));
					  printf ("Log Level: %s \n", levels[pMsg.type]);
					  printf ("Message Data: %s \n", pMsg.msg);
					  status = log_data(&pfile, &log_msg, fileName);
					  break;
				  case HEART_BEAT:
					  log_msg = create_message_struct(LOGGER_THREAD, MAINTHREAD, HEARTBEAT,
							                            HEART_BEAT);
					  info.data = log_msg;
					  info.thread_mutex_lock = main_queue_mutex;
					  info.qName = MAIN_QUEUE;
					  status = msg_send(&info);
					  break;
				  case SHUT_DOWN:
            status = ERROR;
            break; //EXIT CODE
				  default:;
			  }
      }
    }
    sleep(5);
	}
  /* Clean up */
  fclose(pfile);
  pthread_mutex_destroy(&log_queue_mutex);
  mq_unlink(LOGGER_QUEUE);
  pthread_exit();
}
