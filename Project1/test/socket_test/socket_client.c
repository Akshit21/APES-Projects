#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "message.h"

#define PORT_NO (9999)

int main(int argc, char const *argv[])
{
  if(argc!=2)
  {
    perror("Not enough command line arguments.\n");
    exit(EXIT_FAILURE);
  }
  int socketfd, portno=PORT_NO;
  struct sockaddr_in serv_addr;
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if(socketfd <0)
  {
    perror("Failed to create socket.\n");
    exit(EXIT_FAILURE);
  }
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(portno);
  if(connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==0)
  {
    msg_t msg_out, msg_in;
    msg_out.msg_id = MSG_ID_LED;
    msg_out.msg_payload.led_ctl = (int)(*argv[1]-48);
    msg_out.payload_len = 1;
    printf("Client: Sending LED control command.\n");
    write(socketfd, &msg_out, sizeof(msg_out));
    sleep(2);
    read(socketfd, &msg_in, sizeof(msg_in));
    process_msg(msg_in);
    shutdown(socketfd, SHUT_RDWR);
  }
  else
  {
    perror("Failed to connect.\n");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
