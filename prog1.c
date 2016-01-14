//Written by Michael Giancola for CS3516 A15
//Some code is borrowed from D&C Textbook

#include "prog1.h"

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

//Converses with one user
//Returns a pointer to the head of the database if we're waiting for another user
//Returns NULL if we're done
struct Database* HandleTCPClient(int clntSocket, struct Database* list) {
  char recvBuffer[RCVBUFSIZE];
  char sendBuffer[RCVBUFSIZE];
  char tokBuffer [RCVBUFSIZE];
  char* token;
  int bytesRcvd, totalBytesRcvd;
  int recvMsgSize;
  char usrName[6]; //Either FEMA or Query
  int numCmds = 0; //Number of commands usrName issued
  int numPack = 0; //Number of packets sent by server
  FILE* fp = fopen("MEDatabase.txt", "w");

  token = (char*) malloc(RCVBUFSIZE);


  while(1) { //Run forever; quit will eventually end the function
    bytesRcvd = 0;
    totalBytesRcvd = 0;
    while (totalBytesRcvd < RCVBUFSIZE) {
      //Receive up to the buffer size
      if ((bytesRcvd = recv(clntSocket, recvBuffer, RCVBUFSIZE, 0))<=0)
        DieWithError("recv() failed or connection closed prematurely");
      totalBytesRcvd += bytesRcvd; //Keep tally of total bytes
    }
    numCmds++; //Received a command
    list->numCmds++;

    strncpy(tokBuffer, recvBuffer, RCVBUFSIZE); //Copy buffer to be tokenized
    token = strtok(tokBuffer, " "); //Token is command

    if(strncasecmp(token, "login", 5)==0) {
      token = strtok(NULL, " "); //token is user's name
      token[strlen(token)-1] = '\0'; //remove newline character
      strcpy(usrName, token);        //Store user name
      sprintf(sendBuffer, "S: Hello %s!\n", usrName);
      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
          DieWithError("send() failed");
      numPack++;
      list->numPack++;
    }
    //Add Command
    else if (strncasecmp(token, "add", 3)==0) {
      char id_number[10], first_name[21], last_name[26], gender, location[31];
      token = strtok(NULL, " "); //Token is id_number
      strncpy(id_number, token, 9);
      token = strtok(NULL, " "); //Token is first name
      strncpy(first_name, token, 20);
      token = strtok(NULL, " "); //Token is last name
      strncpy(last_name, token, 25);
      token = strtok(NULL, " "); //Token is gender
      gender = token[0];
      token = strtok(NULL, " "); //Token is location
      token[strlen(token)-1] = '\0'; //remove newline character
      strncpy(location, token, 30);
      id_number[10] = first_name[21] = last_name[26] = location[31] = '\0'; //Ensure string is terminated correctly

      struct Node* person = createNode(id_number, first_name, last_name, gender, location);
      if (inList(list->head, person))
        sprintf(sendBuffer, "ERROR: id_number already in database\n");
      else if (list->head == NULL) {
        list->head = person;
        sprintf(sendBuffer, "S: %.9s %s %s %c %s\n", id_number, first_name, last_name, gender, location);
      }
      else {
        list->head = addNode(list->head, person);
        sprintf(sendBuffer, "S: %.9s %s %s %c %s\n", id_number, first_name, last_name, gender, location);
      }

      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE) //Send same info back
          DieWithError("send() failed");
      numPack++;
      list->numPack++;
    }

    //Update Command
    else if (strncasecmp(token, "update", 6)==0) {
      char id_number[10], first_name[21], last_name[26], gender, location[31];
      token = strtok(NULL, " "); //Token is id_number
      strncpy(id_number, token, 9);
      token = strtok(NULL, " "); //Token is first name
      strncpy(first_name, token, 20);
      token = strtok(NULL, " "); //Token is last name
      strncpy(last_name, token, 25);
      token = strtok(NULL, " "); //Token is gender
      gender = token[0];
      token = strtok(NULL, " "); //Token is location
      token[strlen(token)-1] = '\0'; //remove newline character
      strncpy(location, token, 30);
      id_number[10] = first_name[21] = last_name[26] = location[31] = '\0'; //Ensure string is terminated correctly

      struct Node* person = getNode(list->head, id_number);
      if(person != NULL) { //Person is in the list
        list->head = removeNode(list->head, person); //Remove them
        person = createNode(id_number, first_name, last_name, gender, location); //Updated info
        if(list->head == NULL)
          list->head = person;
        else addNode(list->head, person); //Add them back in the correct place
        sprintf(sendBuffer, "S: %.9s %s %s %c %s\n", person->id_number, person->first_name, person->last_name, person->gender, person->location);
      }
      else
        sprintf(sendBuffer, "S: ERROR: ID Number not found in database\n");

      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE) //Send same info back
          DieWithError("send() failed");
      numPack++;
      list->numPack++;
    }

    //Remove Command
    else if (strncasecmp(token, "remove", 6)==0) {
      char id_number[10];
      token = strtok(NULL, " "); //Token is id_number
      strncpy(id_number, token, 9);
      token[10] = '\0';

      struct Node* person = getNode(list->head, id_number);
      if (person != NULL) {
        sprintf(sendBuffer, "S: %s %s was removed from the database\n", person->first_name, person->last_name);
        list->head = removeNode(list->head, person);
      }
      else
        sprintf(sendBuffer, "S: ERROR: ID Number not found in database\n");

      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE) //Send same info back
          DieWithError("send() failed");
      numPack++;
      list->numPack++;
    }

    //Find Command
    else if (strncasecmp(token, "find", 4)==0) {
      char first_name[21], last_name[26];
      token = strtok(NULL, " "); //Token is first name
      strncpy(first_name, token, 21);
      token = strtok(NULL, " "); //Token is last name
      token[strlen(token)-1] = '\0'; //Remove newline
      strncpy(last_name, token, 26);
      struct Node* temp = list->head;
      int numFound = 0;

      while(temp != NULL) {
        if(strcasecmp(temp->first_name, first_name)==0 && strcasecmp(temp->last_name, last_name)==0) {
          numFound++;
          sprintf(sendBuffer, "S: %.9s %s %s %c %s\n", temp->id_number, temp->first_name, temp->last_name, temp->gender, temp->location);
          if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
              DieWithError("send() failed");
          numPack++;
          list->numPack++;
        }
        temp = temp->next;
      }
      if (numFound == 0) {
        sprintf(sendBuffer, "S: No entries were found matching that criteria.\n");
        if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
            DieWithError("send() failed");
        numPack++;
        list->numPack++;
      }
      sprintf(sendBuffer, "S: Search Complete.");
      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
          DieWithError("send() failed");
      //numPack++;       Didn't include these so as to not confuse TA's
      //list->numPack++;
    }

    //List Command
    else if (strncasecmp(token, "list", 4)==0) {
      int numFound = 0;
      char first, last;
      struct Node* person = list->head;
      token = strtok(NULL, " "); //Token is EITHER first or NULL
      if (token == NULL) {
        while(person != NULL) { //Send the entire database
          numFound++;
          sprintf(sendBuffer, "S: %.9s %s %s %c %s\n", person->id_number, person->first_name, person->last_name, person->gender, person->location);
          if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
              DieWithError("send() failed");
          numPack++;
          list->numPack++;
          person = person->next;
        }
      }
      else {
        first = token[0];
        token = strtok(NULL, " "); //Token is EITHER finish or NULL
        if (token == NULL) {
          while(person != NULL) { //Search based on FIRST
            if (strncasecmp(&person->last_name[0], &first, 1)==0 && (strncasecmp(person->last_name, "UNK", 3)!=0)) {
              numFound++;
              sprintf(sendBuffer, "S: %.9s %s %s %c %s\n", person->id_number, person->first_name, person->last_name, person->gender, person->location);
              if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
                  DieWithError("send() failed");
              numPack++;
              list->numPack++;
            }
            person = person->next;
          }
        }
        else { //Search based on LAST
          last = token[0];
          if (strncmp(&first, &last, 1)<0) {
            numFound++; //So that the "No Entries" message doesn't print
            sprintf(sendBuffer, "S: ERROR: Finish is lower than Start\n");
            if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
                DieWithError("send() failed");
            numPack++;
            list->numPack++;
          }
          else {
            while(person != NULL) {
              if (strncasecmp(&person->last_name[0], &first, 1)>=0 && strncasecmp(&last, &person->last_name[0], 1)>=0 && (strncasecmp(person->last_name, "UNK", 3)!=0)) {
                numFound++;
                sprintf(sendBuffer, "S: %.9s %s %s %c %s\n", person->id_number, person->first_name, person->last_name, person->gender, person->location);
                if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
                    DieWithError("send() failed");
                numPack++;
                list->numPack++;
              }
              person = person->next;
            }
          }
        }
      }
      if (numFound == 0) {
        sprintf(sendBuffer, "S: No entries were found matching that criteria.\n");
        if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
            DieWithError("send() failed");
        numPack++;
        list->numPack++;
      }
      sprintf(sendBuffer, "S: Search Complete.");
      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
          DieWithError("send() failed");
          //numPack++;       Didn't include these so as to not confuse TA's
          //list->numPack++;
    }

    //Locate Command
    else if (strncasecmp(token, "locate", 6)==0) {
      token = strtok(NULL, " "); //Token is location
      token[strlen(token)-1] = '\0'; //Remove newline
      struct Node* temp = list->head;
      int numFound = 0;

      while(temp != NULL) {
        if(strcasecmp(temp->location, token) == 0) {
          numFound++;
          sprintf(sendBuffer, "S: %.9s %s %s %c %s\n", temp->id_number, temp->first_name, temp->last_name, temp->gender, temp->location);
          if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
              DieWithError("send() failed");
          numPack++;
          list->numPack++;
        }
        temp = temp->next;
      }
      if (numFound == 0) {
        sprintf(sendBuffer, "S: No entries were found matching that criteria.\n");
        if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
            DieWithError("send() failed");
        numPack++;
        list->numPack++;
      }
      sprintf(sendBuffer, "S: Search Complete.");
      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0) != RCVBUFSIZE)
          DieWithError("send() failed");
      //numPack++;       Didn't include these so as to not confuse TA's
      //list->numPack++;
    }

    //Quit Command
    else if (strncasecmp(token, "quit", 4)==0) {
      token = strtok(NULL, " ");        //Token is EOF marker (if passed)

      sprintf(sendBuffer, "S: TCP Connection will be closed for %s\n", usrName);
      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
        DieWithError("send() sent a different nuber of bytes than expected");
      numPack++;
      list->numPack++;

      sprintf(sendBuffer, "S: ME Server received %d commands from %s\n", numCmds, usrName);
      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
        DieWithError("send() sent a different nuber of bytes than expected");
      numPack++;
      list->numPack++;

      numPack++; //Pre-emptively counting the packet about to be sent
      list->numPack++;
      sprintf(sendBuffer, "S: ME Server sent %d TCP packets to %s\n", numPack, usrName);
      if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
        DieWithError("send() sent a different nuber of bytes than expected");

      printf("ME Server sent %d TCP packets to the client\n", numPack);

      if (token != NULL && strncmp(token, "EOF", 3)==0) {

        sprintf(sendBuffer, "S: ME Server received %d commands in total\n", list->numCmds);
        if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
          DieWithError("send() sent a different nuber of bytes than expected");
        list->numPack++;

        list->numPack++; //Pre-emptively counting the packet about to be sent
        sprintf(sendBuffer, "S: ME Server sent %d TCP packets in total\n", list->numPack);
        if (send(clntSocket, sendBuffer, RCVBUFSIZE, 0)!= RCVBUFSIZE)
          DieWithError("send() sent a different nuber of bytes than expected");

        writeToFile(list, fp); //Writes the database out to a file
        close(clntSocket); //Close user's connection
        fclose(fp);
        return NULL; //If EOF string passed, close connection
      }
      else {
        close(clntSocket); //Close user's connection
        fclose(fp);
        return list; //EOF string was not passed; continue waiting for new user
      }
    }
  }
}

//Writes the database out to file fp
//Assumes fp is already open for writing
void writeToFile(struct Database* list, FILE* fp) {
  if(list->head == NULL)
    printf("List Empty. Nothing to write to file.\n");
  else {
    struct Node* elt = list->head;
    fprintf(fp, "%-9s %-20s %-25s %s %-30s\n", "ID Number", "First Name", "Last Name", "Gender", "Location");
    while(elt != NULL) {
      printNode(elt, fp);
      elt = elt->next;
    }
  }
}

//Prints a single Node to fp
//Assumes fp is already open for writing
void printNode(struct Node* elt, FILE* fp) {
  fprintf(fp, "%-.9s %-20s %-25s %c   %-30s\n", elt->id_number, elt->first_name, elt->last_name,
          elt->gender, elt->location);
}
