#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void qinit(queue *q) {
    q -> head = NULL;
    q -> tail = NULL;
}


void enq(queue *q, char *s) {
	node *newnode, *ptr;
	if(qempty(q)) {
		qinit(q); // Reinitializing list if list is empty by deq function
	}
	newnode = (node*)malloc(sizeof(struct node));
	if(!newnode) { 
		full = 1;// global variable full will set if malloc fails.
	}
	if(!qfull(q)) {
		newnode -> s = s;
		newnode -> next = NULL;
		ptr = q -> tail;

		if(!ptr) {
			q -> head = newnode;
			q -> tail = newnode;
		}
		else {
			ptr = q -> tail;
			ptr -> next = newnode;
			q -> tail = newnode;
		}
	}
}

char* deq(queue *q) {
	if(!qempty(q)) {
		char *s;
		node *ptr = q -> head;
		s = ptr -> s;
		q -> head = q -> head -> next;
		ptr -> next = NULL;
		free(ptr);
		return s;
	}
	return NULL;
}


int qfull(queue *q) {
	if(full == 1) {
		full = 0;
		return 1;//queue full if malloc fails
	}
	else
		return 0; //queue never full when implemented using linked list
}

int qempty(queue *q) {
    return q -> head == NULL;
}

