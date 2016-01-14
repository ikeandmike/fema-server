// Written by Michael Giancola for CS 3516 A15
// Some code is borrowed from D&C Textbook

#include "prog1.h"

int main(int argc, char* argv[]) {
  int servSock; //Socket Descriptor for server
  int clntSock; //Socket Descriptor for client
  struct sockaddr_in servAddr; //Local address
  struct sockaddr_in clntAddr; //Client address
  unsigned short servPort; //Server port
  unsigned int clntLen; //Length of client address data structure
  int doneFlag = -1;

  servPort = 1100; //Server port

  //Create socket for incoming connections
  if((servSock = socket(AF_INET,SOCK_STREAM,0))<0)
    DieWithError("socket() failed");

  //Construct local address structure
  memset(&servAddr, 0, sizeof(servAddr));   //Zero out structure
  servAddr.sin_family = AF_INET;                //Internet address family
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Any incoming interface
  servAddr.sin_port = htons(servPort);      //Local port

  //Enable reuse of the port by local addresses
  //This allows the program to bind to the port even if it wasn't closed correctly
  //the last time it was used
  if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    DieWithError("setsockopt() failed");

  //Bind to the local address
  if (bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr))<0)
    DieWithError("bind() failed");

  //Mark the socket so it will listen for incoming connections
  if (listen(servSock, MAXPENDING)<0)
    DieWithError("listen() failed");

  //Set the size of the in-out parameter
  clntLen = sizeof(clntAddr); //Wait for a client to connect

  struct Node* list_head = NULL;
  struct Database* list = (struct Database*) malloc(sizeof(struct Database));
  list->head = list_head;

  while(list != NULL) {
    if ((clntSock = accept(servSock, (struct sockaddr*)&clntAddr,&clntLen))<0)
      DieWithError("accept() failed");

    list = HandleTCPClient(clntSock, list); //Handle One User
  }
  close(servSock);
}
