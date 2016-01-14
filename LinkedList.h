// Written by Michael Giancola for CS3516 A15

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Maintains a static pointer to the head of the list
struct Database {
  struct Node* head;
  int numCmds; //Total number of cmds (all users)
  int numPack; //Total number of packets (all users)
};

//Node structure for a LinkedList
struct Node {
  char id_number[10];
  char first_name[21];
  char last_name[26];
  char gender;
  char location[31];

  struct Node* next; //Each node points to the next node in the list; NULL if tail
};

//Function Prototypes for Functions in LinkedList.c
struct Node* createNode(char* id_number, char* first_name, char* last_name, char gender, char* location);
struct Node* addNode(struct Node* head, struct Node* elt);
struct Node* addNode_r(struct Node* head, struct Node* elt, struct Node* prev);
void moveToEnd(struct Node* head, struct Node* elt);
struct Node* removeNode(struct Node* head, struct Node* elt);
void removeNode_r(struct Node* head, struct Node* elt, struct Node* prev);
struct Node* getNode(struct Node* head, char* id_number);
int inList(struct Node* head, struct Node* elt);

#endif
