#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "banking.h"

#define NUM_ACCTS   100

typedef struct {
  unsigned int 	balance;
  pthread_mutex_t mutex;
} sBANK_ACCT_DATA;

sBANK_ACCT_DATA acctData[NUM_ACCTS];

/*We receive a Protocol from the client so we declare the struct*/
sBANK_PROTOCOL s;

int handleClient(int clientSocket) {
  int msgSize;

  /* Clear the structure that we are going to be receiving*/
  memset(&s, 0, 1500);

  if((msgSize = recv(clientSocket, &s, sizeof(s), 0)) < 0) {
    printf("Failed to receive on client socket; Error %d: %s\n", errno, strerror(errno));
    return -1;
  }

  /* Printing the contents of our received protocol from the client*/
  printf("RECEIVED : \n");
  printf("  Transaction: %u\n", s.trans);
  printf("  Account Number: %u\n", s.acctnum);
  printf("  Value: %u\n", s.value );
  printf("\n");
  printf("---------------------------\n");
  printf("\n");

  /* Performing the desired transaction */
  switch(s.trans){
    default:      //Invalid transaction
      printf("Trying to perform invalid transaction\n");
      break;
    case 0:       //Deposit
      acctData[s.acctnum].balance = acctData[s.acctnum].balance + s.value;
      break;
    case 1:       //Withdrawal
      if (acctData[s.acctnum].balance <= s.value) //If our balance is negative set it to 0
        acctData[s.acctnum].balance = 0;
      else
        acctData[s.acctnum].balance = acctData[s.acctnum].balance - s.value;
      break;
    case 2:       //Balance Inquiry
      s.value = acctData[s.acctnum].balance;     //Return our balance
      break;
  } 

  /* Sending the protocol back to the client */
  send(clientSocket, &s, sizeof(s),0);

  close(clientSocket);
  return 0;
}

int setupTCPServer(unsigned int portNum, int maxConn) {
  int servSocket;
  struct sockaddr_in servAddr;

  /* Setup address */
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(0);  /* Accept connections on an IP address */
  servAddr.sin_port = htons(portNum);

  /* Create socket */
  if((servSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("Failed to create socket; Error %d: %s\n", errno, strerror(errno));
    return -1;
  }

  /* Bind socket for server */
  if(bind(servSocket,(struct sockaddr *) &servAddr,sizeof(servAddr)) < 0) {
    printf("Failed to bind socket; Error %d: %s\n", errno, strerror(errno));
    return -1;
  }

  /* Setup socket for listening */
  if((listen(servSocket, maxConn)) < 0) {
    printf("Failed to listen on socket; Error %d: %s\n", errno, strerror(errno));
    return -1;
  }

  /* return the socket handle */
  return servSocket;

}

int main(int argc, char **argv) {
  int mySocket, clientSocket;
  struct sockaddr_in clientAddr;
  unsigned int clientLen = sizeof(clientAddr);
  unsigned int portNum;

  if(argc != 2) {
    printf("Usage: bankServer portNum\n");
    return -1;
  }

  /* Setup TCP port number */
  portNum = atoi(argv[1]);

  /* Setup the server TCP Socket */
  if((mySocket = setupTCPServer(portNum, 5)) < 0) {
    return -1;
  }

  for(;;) {
    if((clientSocket = accept(mySocket, (struct sockaddr *) &clientAddr, &clientLen)) < 0) {
      printf("Failed to accept connection from client; Error %d: %s\n", errno, strerror(errno));
      return -1;
    }
    /* Handling the socket and performing operations */
    handleClient(clientSocket);
  }

  return 0;
}
