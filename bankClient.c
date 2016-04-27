#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "banking.h"

int setupTCPClient(char *servIPAddr, unsigned int portNum) {
  int clientSocket;
  struct sockaddr_in servAddr;

  /* Setup address of server */
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(servIPAddr);
  servAddr.sin_port = htons(portNum);

  /* Create socket */
  if((clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("Failed to create socket; Error %d: %s\n", errno, strerror(errno));
    return -1;
  }

  /* Connect socket to server */
  if(connect(clientSocket,(struct sockaddr *) &servAddr,sizeof(servAddr)) < 0) {
    printf("Failed to connect socket to %s:%d; Error %d: %s\n", servIPAddr, portNum, errno, strerror(errno));
    return -1;
  }

  return clientSocket;
}

int main(int argc, char **argv) {
  int mySocket;
  char serverIP[15];
  char recvBuff[1024];
  unsigned int portNum;
  int command, accNum, value;
  char sendString[100];
  int n;

  if(argc != 6) {
    printf("Usage: bankClient servIPAddr servPortNum command acctNum value\n");
    return -1;
  }

  /* Setup the IP address */
  strcpy(serverIP, argv[1]);

  /* Setup TCP port number */
  portNum = atoi(argv[2]);

  /* Setup the command to the server */
  command = atoi(argv[3]);
  accNum  = atoi(argv[4]);
  value   = atoi(argv[5]);

  /* Setup client socket */
  if((mySocket = setupTCPClient(serverIP, portNum)) < 0) {
    printf("Could not establish connection to server!\n");
    return -1;
  }
  accNum = htonl(accNum);
  value = htonl(value);

  sendString[0] = '\0';
  int i;
  for (i=3; i < argc; i++) 
  {
    /* Setup the message */
    sprintf(sendString, "%s %s", sendString, argv[i]);    
  }
  sprintf(sendString, "%s\n", sendString);

  /* Send string to server */
  send(mySocket, sendString, strlen(sendString), 0);
  printf("Sent:\n%s\n", sendString);

  /* Receive a string from a server */
  recv(mySocket, recvBuff, 1023, 0);
  printf("Received:\n%s\n", recvBuff);

  close(mySocket);
}
