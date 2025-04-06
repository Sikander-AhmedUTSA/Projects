#include "sjf.h"
#include <stdlib.h>


PCB* sjfGetNextProcess(LinkedList* readyQueue) {
	if (!readyQueue || readyQueue->head)
		return NULL;

	// Traverse the linked list and find the shortest job!
	// Start at the head first though
	PCB* bestPCB = readyQueue->head;
	int bestBurst = bestPCB->CPUBurst[bestPCB->cpuIndex];

	//while (curr->next != NULL) but in a for loop!
	for (PCB* curr = bestPCB->next; curr != NULL; curr = curr->next) {
		int currBurst = curr->CPUBurst[curr->cpuIndex];
		if (currBurst < bestBurst) {
			bestBurst = currBurst;
			bestPCB = curr;
		}
	}

	// Remove the best PCB from the list and updatea the pointers
	if (bestPCB == readyQueue->head) { // If first elements
		readyQueue->head = bestPCB->next;
		if (readyQueue->head)
			readyQueue->head->prev = NULL;
		else
			readyQueue->tail = NULL;

	} else {
		if (bestPCB->prev) // Setting the prev PCB to point to the one after best
			bestPCB->prev->next = bestPCB->next;
		if (bestPCB->next) // Setting the next PCB to point to the one before best
			bestPCB->next->prev = bestPCB->prev;
		if (bestPCB == readyQueue->tail) // If at the end
			readyQueue->tail = bestPCB->prev;
	}
	readyQueue->size--;
	bestPCB->next = bestPCB->prev = NULL;
	return bestPCB;
	
}
