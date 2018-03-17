#include "project.h"

void * task_socket(void* param)
{
  int socketfd, newsocketfd, portno=PORT_NO;
  /* Create a TCP/IP socket */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if(socketfd<0)
  {
    perror("Failed to create a TCP/IP socket.\n");
    exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
  }
  /* Listen to incoming connections
     The listen() function call places all incoming connection into a backlog queue
     until accept() call accepts the connection.
     The maximum size for the backlog queue is set to 5 */
  listen(socketfd, 5);
  while((newsocketfd = accept(socketfd, NULL, NULL)))
  {
    printf("got it\n");
    //pthread_create(&task, 0, process, (void*)newsocketfd);
  }
  /* Exit success */
  exit(EXIT_SUCCESS);
}
//
// void * process(void* fd)
// {
//   int servicefd = (int)fd;
//   msg_t msg_out, msg_in;
//   pthread_detach(pthread_self());
//   msg_out.msg_id = MSG_ID_STRING;
//   /* Read data */
//   if(read(servicefd, &msg_in, sizeof(msg_in))==sizeof(msg_in))
//   {
//     /* Process the read data */
//     if(msg_in.msg_id == MSG_ID_LED)
//     {
//       switch (msg_in.msg_payload.led_ctl)
//       {
//         case LED_ON:
//           //ledON(ledPath);
//           printf("Receive LED_ON command from client.\n");
//           sprintf(msg_out.msg_payload.string, "Server: LED is on");
//           msg_out.payload_len = strlen(msg_out.msg_payload.string);
//           write(servicefd, &msg_out, sizeof(msg_out));
//           break;
//         case LED_OFF:
//           //ledOff(ledPath);
//           printf("Receive LED_OFF command from client.\n");
//           sprintf(msg_out.msg_payload.string, "Server: LED is off");
//           msg_out.payload_len = strlen(msg_out.msg_payload.string);
//           write(servicefd, &msg_out, sizeof(msg_out));
//           break;
//         default:;
//       }
//     }
//   }
//   else
//     printf("Failed to read incoming packet.\n");
//   close(servicefd);
// }
