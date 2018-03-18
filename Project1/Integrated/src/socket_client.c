#include "project.h"

#define PORT_NO (9999)

void printMenu(void);
void process_msg(Message_t *pResp);

int main(int argc, char const *argv[])
{
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
  Message_t socket_msg_req = {0};
  Message_t socket_msg_resp = {0};
  socket_msg_req = create_message_struct(SOCKET_CLIENT, SOCKET_THREAD, INFO, REQUEST_MAX);
  printMenu();
  while(1)
  {
  	scanf("%d", &socket_msg_req.requestId);
  	if(connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==0)
  	{
      		write(socketfd, &socket_msg_req, sizeof(socket_msg_req));
      		sleep(10);
      		read(socketfd, &socket_msg_resp, sizeof(socket_msg_resp));
   		printf("read something\n");	
      		shutdown(socketfd, SHUT_RDWR);
      		process_msg(&socket_msg_resp);
  	}
  	else
  	{
    		perror("Failed to connect.\n");
    		exit(EXIT_FAILURE);
  	}
  }
  exit(EXIT_SUCCESS);
}

void printMenu(void)
{
  printf("\n");
  printf("**********************************************************\n");
  printf("Welcome to Weather Station\n");
  printf("**********************************************************\n");
  printf("Choose an Option\n");
  printf("\n(1). Get Temperature (in Celcius)");
  printf("\n(2). Get Temperature (in Farenheit)");
  printf("\n(3). Get Temperature (in Kelvin)");
  printf("\n(4). Get Light Luminosity");
  printf("\n(5). Get Light State");
  printf("\n**********************************************************\n");
  printf("Enter the Option:");
}

void process_msg(Message_t *pResp)
{
  printf("\n[Response]:%s\n[Time]:%s\n",pResp->msg,pResp->timeStamp);
}
