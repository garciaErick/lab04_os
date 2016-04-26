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
  servAddr.sin_family      = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr(servIPAddr);
  servAddr.sin_port        = htons(portNum);

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
  int          mySocket;
  char         serverIP[15];
  unsigned int portNum;

  if(argc != 6) {
    printf("Usage: bankClient servIPAddr servPortNum command acctNum value\n");
    return -1;
  }

  /* Setup the IP address */
  strcpy(serverIP, argv[1]);

  /* Setup TCP port number */
  portNum = atoi(argv[2]);
    
  /* Setup client socket */
  if((mySocket = setupTCPClient(serverIP, portNum)) < 0) {
    printf("Could not establish connection to server!\n");
    return -1;
  }

  /* Declaring the data structure we are going to send */
  sBANK_PROTOCOL s; 

  /* Which command are we using? */
  switch(argv[3][0]){
    default:      //No Viable Input
      printf("Unknown option -%c\n\n", argv[0][0]);
      s.trans = -1;
      return -1;
      break;
    case 'D':     //Deposit
      s.trans = 0;
      break;
    case 'W':     //Withdrawal
      s.trans = 1;
      break;
    case 'B':     //Balance Inquiry
      s.trans = 2;
      break;
  }
  s.acctnum = atoi(argv[4]);
  s.value   = atoi(argv[5]);

  /* Send my struct to server */
  send(mySocket, &s, sizeof(s), 0);

  /* Receive my processed struct from a server */
  recv(mySocket, &s, sizeof(s), 0);

  /* Output */ 
  printf("===========================\n");
  printf("SENT: \n");                       //What we sent to the server
  printf("  Transaction: %s\n", argv[3]);
  printf("  Account Number: %s\n", argv[4]);
  printf("  Value: %s\n", argv[5] );
  printf("\n");
  printf("---------------------------\n");
  printf("\n");                            //What we are getting from the server
  printf("RECEIVED: \n");
  printf("  Account Number: %u\n", s.acctnum);
  printf("  Balance: %u\n", s.value);
  printf("===========================\n");

  close(mySocket);
}
