//Written by Michael Giancola for CS3516 A15
//My implementation of a Singly Linked List

#include "LinkedList.h"

struct Node* createNode(char* id_number, char* first_name, char* last_name, char gender, char* location) {
  struct Node* x = (struct Node*) malloc(sizeof(struct Node));

  strncpy(x->id_number, id_number, 10);
  x->id_number[10] = '\0'; //Ensures id_number is null terminated
  strncpy(x->first_name, first_name, 21);
  strncpy(x->last_name, last_name, 26);
  x->gender = gender;
  strncpy(x->location, location, 31);

  x->next = NULL;

  return x;
}

//Adds elt to head in the correct place (alphabetical order by last name followed
//by chronological order)
//
//Returns the head of the list
//
//This is a wrapper function for addNode_r, which hides some of the details
//from the caller
//
//ASSUMES HEAD IS NOT NULL
struct Node* addNode(struct Node* head, struct Node* elt) {
  return addNode_r(head, elt, NULL);
}

struct Node* addNode_r(struct Node* head, struct Node* elt, struct Node* prev) {

  if (strncmp(elt->last_name, "UNK", 3)==0) {
    moveToEnd(head,elt);
    return head;
  }

  // If elt comes before head alphabetically
  // Or elt comes before head because head is UNK
  if (strncasecmp(elt->last_name,head->last_name,26)<0 || strncasecmp(head->last_name, "UNK", 3)==0) {
    elt->next = head; //Elt will come before head, so it should point to head

    if(prev != NULL)   //If there is a prevoius elt, it should now point to
      prev->next = elt;//elt, as it is placed directly after it

    return elt;
  }

  //If head is the only or last Node in the list
  if (head->next == NULL) {
    head->next = elt;
    return head;
  }
  //If we haven't placed elt yet, move down one Node in the list
  addNode_r(head->next, elt, head);
  return head; //head will ultimately be the start of the list
}

//Puts elt at the end of the list pointed to by head
void moveToEnd(struct Node* head, struct Node* elt) {
  if(head->next == NULL)
    head->next = elt;
  else
    moveToEnd(head->next, elt);
}

//Removes elt from head if it is in the list, and returns the head of the list
//If elt is head, the new head is head->next
struct Node* removeNode(struct Node* head, struct Node* elt) {
  struct Node* temp;
  if (inList(head,elt)) { //Check if elt is in the list
    if (head == elt)  {   //If we want to remove the head
      temp = head->next;
      free(head);         //Free its memory
      return temp;        //Then return the next element in the list
    }
    else { //If not, recursively check the rest of the list
      removeNode_r(head->next, elt, head);
      return head; //Then return the head of the list
    }
  }
  else return head;
}

//Should only be called from removeNode_r
//Assumes that elt is in the list specified by head, and that prev is not NULL
void removeNode_r(struct Node* head, struct Node* elt, struct Node* prev) {
  struct Node* temp;
  if (head == elt) {         //If the head is the elt we want to remove
    prev->next = head->next; //The previous Node should point to the Node after head
    free(head);              //Free memory for head
  }
  else //Otherwise, shift down one Node in the list
    removeNode_r(head->next, elt, head);
}

//Returns the Node with id_number, or NULL if id_number isn't found
struct Node* getNode(struct Node* head, char* id_number) {
  if(head == NULL) return NULL;
  if(strncmp(head->id_number, id_number, 9)==0) return head;

  struct Node* temp = head->next;
  while(temp != NULL) {
    if(strncmp(temp->id_number, id_number, 9)==0) return temp;
    temp = temp->next;
  }
  return NULL;
}

//Returns 1 if elt is in head, 0 if not
int inList(struct Node* head, struct Node* elt) {
  if(head==NULL) return 0;
  else return (getNode(head, elt->id_number)!=NULL) ? 1 : 0;
}
