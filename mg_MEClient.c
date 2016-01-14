// Written by Michael Giancola for CS 3516 A15
// Some code is borrowed from D&C Textbook

#include "prog1.h"

int main(int argc, char* argv[]) {
  int sock;                        //Socket descriptor
  struct sockaddr_in servAddr; //Echo server address
  char* lineBuffer;                //String to send to server
  char* tempBuffer;                //Temporary Buffer for Storage
  char recvBuffer[RCVBUFSIZE];
  int bytesRcvd, totalBytesRcvd;   //Bytes read in single recv & total
  FILE* in;                        //MEClient.txt, if passed
  FILE* out;                       //MEClient.log, logfile of correspondance
  struct hostent* he;              //Used to get IP information from hostname
  int usrAuth = -1;
  char* token;
  int i, doneSig;

  if (argc < 2) { //Test for correct number of arguments
    fprintf(stderr,"Usage: %s <Server Machine Name> [MEClient.txt]\n", argv[0]);
    exit(1);
  }

  if (argc == 3)              //If file is passed in
    in = fopen(argv[2], "r"); //Create a file descriptor for the input file
  else
    in = stdin;               //Otherwise, use Standard In

  out = fopen("MEClient.log", "w"); //Open log file

  if (in == NULL)
    DieWithError("Error Opening File for Reading");

  //Construct the Server Address Structure
  memset(&servAddr, 0, sizeof(servAddr)); //Zero out structure
  if((he = gethostbyname(argv[1])) == NULL)
    DieWithError("Error resolving Server IP");
  memcpy(&servAddr.sin_addr, he->h_addr_list[0], he->h_length); //Set address
  servAddr.sin_family = AF_INET;                                //Family
  servAddr.sin_port = htons(1100);                              //Well-Known Port

  lineBuffer = (char*) malloc(LINEBUFSIZE);
  tempBuffer = (char*) malloc(LINEBUFSIZE);
  token      = (char*) malloc(LINEBUFSIZE);
  while(1) { //Run forever; quit function will eventually end the program
    fgets(lineBuffer, LINEBUFSIZE, in); //Gets line from MEClient.txt
    fprintf(out, lineBuffer);            //Write command to log file
    strncpy(tempBuffer, lineBuffer, LINEBUFSIZE);
    token = strtok(tempBuffer, " ");

    //Login Command
    if(strncasecmp(token, "login", 5)==0) {
      token = strtok(NULL, " ");   //Token is user's name

      //Log in user, and set authorization level
      if(strncmp(token, "FEMA", 4)==0 || strncmp(token, "Query", 5)==0) {
        if(usrAuth != -1)
          puts("ERROR: User already logged in");
        else{
          //Create a reliable, stream socket using TCP
          if ((sock = socket(AF_INET, SOCK_STREAM, 0))<0)
            DieWithError("socket() failed");
          //Establish a connection to the ME Server
          if (connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr))<0)
            DieWithError("connect() failed");
          //Return 1 for FEMA, 0 for Query
          usrAuth = ((strncmp(token, "FEMA", 4)==0) ? 1 : 0);

          if (send(sock, lineBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
            DieWithError("send() sent a different nuber of bytes than expected");
          bytesRcvd = 0;
          totalBytesRcvd = 0;
          while (totalBytesRcvd < RCVBUFSIZE) {
            //Receive up to the buffer size
            if ((bytesRcvd = recv(sock, recvBuffer, RCVBUFSIZE, 0))<=0)
              DieWithError("recv() failed or connection closed prematurely");
            totalBytesRcvd += bytesRcvd; //Keep tally of total bytes
          }
          printf(recvBuffer); //Print server's response
          fprintf(out, recvBuffer); //Write to log file
        }
      }
      else
        puts("ERROR: Invalid login name");
    }

    //Add / Update Command
    else if ((strncasecmp(token, "add", 3)==0 || strncasecmp(token, "update", 6)==0) && usrAuth==1) {
      strcpy(tempBuffer, lineBuffer);
      if (checkVars(tempBuffer, 5)) {
        if (send(sock, lineBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
          DieWithError("send() sent a different nuber of bytes than expected");
        bytesRcvd = 0;
        totalBytesRcvd = 0;
        while (totalBytesRcvd < RCVBUFSIZE) {
          //Receive up to the buffer size
          if ((bytesRcvd = recv(sock, recvBuffer, RCVBUFSIZE, 0))<=0)
            DieWithError("recv() failed or connection closed prematurely");
          totalBytesRcvd += bytesRcvd; //Keep tally of total bytes
        }
        printf(recvBuffer); //Print server's response
        fprintf(out, recvBuffer); //Write to log file
      }
      else
        puts("ERROR: Wrong number of variables.");
    }

    //Remove Command
    else if (strncasecmp(token, "remove", 6)==0 && usrAuth==1) {
      if (send(sock, lineBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
        DieWithError("send() sent a different nuber of bytes than expected");
      bytesRcvd = 0;
      totalBytesRcvd = 0;
      while (totalBytesRcvd < RCVBUFSIZE) {
        //Receive up to the buffer size
        if ((bytesRcvd = recv(sock, recvBuffer, RCVBUFSIZE, 0))<=0)
          DieWithError("recv() failed or connection closed prematurely");
        totalBytesRcvd += bytesRcvd; //Keep tally of total bytes
      }
      printf(recvBuffer); //Print server's response
      fprintf(out, recvBuffer); //Write to log file
    }

    //Find / List / Locate Command
    else if ((strncasecmp(token, "find", 4)==0 || strncasecmp(token, "list", 4)==0 || strncasecmp(token, "locate", 6)==0) && usrAuth!=-1) {
      strcpy(tempBuffer, lineBuffer);
      if(strncasecmp(token, "locate", 6)==0 && checkTooMany(tempBuffer, 2)==1) {
        puts("ERROR: Wrong number of variables.");
        fprintf(out, "ERROR: Wrong number of variables.\n");
        continue;
      }

      if (send(sock, lineBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
        DieWithError("send() sent a different nuber of bytes than expected");

      doneSig = 1;
      while(doneSig) {
        bytesRcvd = 0;
        totalBytesRcvd = 0;
        while (totalBytesRcvd < RCVBUFSIZE) {
          //Receive up to the buffer size
          if ((bytesRcvd = recv(sock, recvBuffer, RCVBUFSIZE, 0))<=0)
            DieWithError("recv() failed or connection closed prematurely");
          totalBytesRcvd += bytesRcvd; //Keep tally of total bytes
        }
        if(strcasecmp(recvBuffer, "S: Search Complete.") == 0)
          doneSig = 0; //Done Receiving
        else {
          printf(recvBuffer); //Print server's response
          fprintf(out, recvBuffer); //Write to log file
        }
      }
    }

    //Quit Command
    else if (strncasecmp(token, "quit", 4)==0 && usrAuth!=-1) {
      token = strtok(NULL, " ");        //Token is EOF marker (if passed)
      usrAuth = -1;                     //Reset authentication level

      if (send(sock, lineBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
        DieWithError("send() sent a different nuber of bytes than expected");

      for(i=0; i<3; i++) { //Receive and print two messages from the server
        bytesRcvd = 0;
        totalBytesRcvd = 0;
        while (totalBytesRcvd < RCVBUFSIZE) {
          //Receive up to the buffer size
          if ((bytesRcvd = recv(sock, recvBuffer, RCVBUFSIZE, 0))<=0)
            DieWithError("recv() failed or connection closed prematurely");
          totalBytesRcvd += bytesRcvd; //Keep tally of total bytes
        }
        printf(recvBuffer);
        fprintf(out, recvBuffer); //Write to log file
      }

      if (token != NULL && strncmp(token, "EOF", 3)==0) {

      for(i=0; i<2; i++) { //Receive and print two messages from the server
        bytesRcvd = 0;
        totalBytesRcvd = 0;
        while (totalBytesRcvd < RCVBUFSIZE) {
          //Receive up to the buffer size
          if ((bytesRcvd = recv(sock, recvBuffer, RCVBUFSIZE, 0))<=0)
            DieWithError("recv() failed or connection closed prematurely");
          totalBytesRcvd += bytesRcvd; //Keep tally of total bytes
        }
        printf(recvBuffer);
        fprintf(out, recvBuffer); //Write to log file
      }

        close(sock);
        fclose(in);
        fclose(out);
        exit(0); //If EOF string passed, we're done
      }
      else
        close(sock);
    }

    else {                       //User either entered a bad command, or they
      puts("ERROR: Bad Command");//had the wrong authentication level
      fprintf(out, "ERROR: Bad Command\n");
    }
  }
}

//Uses strtok to tokenize message
//Returns 1 if message is able to be tokenized numToks times
//Returns 0 if not
int checkVars(char* message, int numToks) {
  int i;
  strtok(message, " ");
  for(i=1; i<numToks; i++) {
    if(strtok(NULL, " ") == NULL)
      return 0;
  }
  return 1;
}

//Similar to checkVars, but if we want to check for too many vars
//numToks is the number of times it SHOULD be tokenizable
//Returns 1 if too many were passed
//Returns 0 if okay
int checkTooMany(char* message, int numToks) {
  int i;
  strtok(message," ");
  for(i=1; i<numToks; i++) {
    if(strtok(NULL, " ") == NULL)
      return 0;
  }
  if (strtok(NULL, " ") != NULL) return 1;
  else return 0;
}
