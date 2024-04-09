#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "globals.h"

//typedef struct node {
//	void *data;
//	struct node *next;
//} node;


// struct node {
	// void *data;
	// struct node *next;
// };
// typedef struct node node;



// void display(node *head); /* displays all the elements of the list. */
node *addnode(node *head, void *data);  /* adds element with specified data to the end of the list and returns new head node. */
node *find(node *head, void *data);     /* returns pointer of element with specified data in list. */
node *delnode(node *head, node *ep);    /* removes element from the list and returns new head node. */
void freelist(node *head);              /* deletes all elements of the list. */

#endif
