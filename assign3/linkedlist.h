#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "pcb.h"

typedef struct LinkedList {
	PCB* head;
	PCB* tail;
	int size;
} LinkedList;

LinkedList* createList();
void insertTail(LinkedList *list, PCB* newPCB);
void insertSorted(LinkedList *list, PCB *newPCB);
PCB* removeHead(LinkedList *list);
PCB* removeHighestPriority(LinkedList *list); // For Priority Queue
int isEmpty(LinkedList *list);
void freeList(LinkedList *list);


#endif
