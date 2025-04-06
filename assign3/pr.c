#include "pr.h"

// VERY similar to Shortest job first, just copied it over, forgive any mistakes!
PCB* prGetNextProcess(LinkedList* readyQueue) {
	if (!readyQueue || readyQueue->head)
		return NULL;

	// Traverse the linked list and find the shortest job!
	// Start at the head first though
	PCB* bestPCB = readyQueue->head;
	int bestPriority = bestPCB->priority;

		//while (curr->next != NULL) but in a for loop!
		for (PCB* curr = bestPCB->next; curr != NULL; curr = curr->next) {
			if (curr->priority > bestPriority) {
				bestPriority = curr->priority;
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
