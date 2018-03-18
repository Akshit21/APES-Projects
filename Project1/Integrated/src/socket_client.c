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
* @file : socket_client.c
* @brief : Client routine for requesting data from the system via socket
*
* @tool : Compiler - GCC, Linker - GDB, Cross Compiler - arm-linux-gnueabihf-gcc
* @hardware : Beagle Bone Green AM335x Arm Corex - A8, TMP106, APDS-9301
* @reference :
***************************************************************************************************/
#include "project.h"

#define PORT_NO (9999)

/* Function declaration */
static inline void printMenu(void);
static inline void process_msg(Message_t *pResp);

/* Client Socket */
int main(int argc, char const *argv[])
{
  int socketfd, portno=PORT_NO;
  struct sockaddr_in serv_addr;
  /* Create a socket */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if(socketfd <0)
  {
    perror("Failed to create socket.\n");
    exit(EXIT_FAILURE);
  }
  /* Server Port attributes*/
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(portno);

  /* Message structures for request and response */
  Message_t socket_msg_req = {0};
  Message_t socket_msg_resp = {0};
  socket_msg_req = create_message_struct(SOCKET_CLIENT, SOCKET_THREAD, INFO, REQUEST_MAX);

  /* Print the Menu */
  printMenu();
  while(1)
  {
    /* Get the user input */
    printf("\nEnter the Option:");
  	scanf("%d", &socket_msg_req.requestId);

    /* Connect to the server */
    if(connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==0)
  	{
      /* Send the message request */
      write(socketfd, &socket_msg_req, sizeof(socket_msg_req));
      sleep(2);

      /* Try reading the response 0*/
      read(socketfd, &socket_msg_resp, sizeof(socket_msg_resp));
   		//printf("read something\n");
    	shutdown(socketfd, SHUT_RDWR);

      /* Print the response */
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

/* Function to print Menu for Client request */
static inline void printMenu(void)
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
}

/* Function to process Message and display */
static inline void process_msg(Message_t *pResp)
{
  printf("\n[Response]:%s\n[Time]:%s\n",pResp->msg,pResp->timeStamp);
}
