// Written by Michael Giancola for CS3516 A15
// prog1.c and prog1.h contain functions, constant definitions,
// and includes that are common between the server and client programs.

#ifndef PROG1_H
#define PROG1_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include "LinkedList.h"

#define MAXPENDING 5    // Maximum outstanding connection requests
#define RCVBUFSIZE 256  // Size of receive buffer
#define LINEBUFSIZE 256 // Size of line buffer (reading from file)

// In prog1.c
void DieWithError(char* errorMessage); // Error handling function
struct Database* HandleTCPClient(int clntSocket, struct Database* list);   // TCP client handling function
void writeToFile(struct Database* list, FILE* fp); //Writes database to a file
void printNode(struct Node* elt, FILE* fp);        //Writes a single Node to file

// In mg_MEClient.c
int checkVars(char* message, int numToks);
int checkTooMany(char* message, int numToks);

#endif
