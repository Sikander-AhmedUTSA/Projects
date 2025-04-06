#include "linkedlist.h"


LinkedList* createList() { // Creates the linked list
	LinkedList *list = (LinkedList*)malloc(sizeof(LinkedList));
	if (!list) {
		perror("Failed to allocate memory for LinkedList");
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	return list;

}

void insertTail(LinkedList *list, PCB* newPCB) { // Inserts pcb into the tail of list
	if (!list || !newPCB)	
		return;


	newPCB->next = NULL;
	newPCB->prev = list->tail; // Mapping previous to the same as the list end

	if (list->tail)
		list->tail->next = newPCB; // End of list
	else
		list->head = newPCB; // If list was empty

	list->tail = newPCB; // Pointing tail to new pcb
	list->size++;

}

void insertSorted(LinkedList *list, PCB *newPCB) { // Sort for priority
	if (!list || !newPCB)
		return;
	if (!list->head || newPCB->priority > list->head->priority) { // If list is empty or newPCB priority is greatest
		newPCB->next = list->head;
		newPCB->prev = NULL;
		if (list->head)
			list->head->prev = newPCB; // Set current list head to point back to newPCB
		list->head = newPCB;
		if(!list->tail)
			list->tail = newPCB; // If list is empty	
	} else { // Find the right position
		PCB *curr = list->head;
		while (curr->next && newPCB->priority <= curr->next->priority) { // Traverse through list
			curr = curr->next;
		}

		newPCB->next = curr->next;
		newPCB->prev = curr;
		if (curr->next) // Setting next pcb to point back to newPCB
			curr->next->prev = newPCB;
		curr->next = newPCB;
		if (!newPCB->next) // Last element
			list->tail = newPCB; 
	}

	list->size++;
}

PCB* removeHead(LinkedList *list) {
	if (!list || !list->head)
		return NULL;
	PCB* temp = list->head;
	list->head = temp->next;
	if (list->head)
		list->head->prev = NULL;
	else // If only 2 elements in list
		list->tail = NULL; 
	list->size--;
	temp->next = NULL;
	temp->prev = NULL;
	return temp;

}

PCB* removeHighestPriority(LinkedList *list){ // For Priority Queue
					      //Remove first from the list really
	removeHead(list);
}

int isEmpty(LinkedList *list) { // 1 for is empty, else 0
	return (list->size == 0);
}


void freeList(LinkedList *list) {
	while (!isEmpty(list)) 
		freePCB(removeHead(list)); // Free's each PCB
	free(list);
}

