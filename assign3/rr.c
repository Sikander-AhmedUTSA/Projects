#include "rr.h"

// Same as FIFO, easy peasy, just remove the next one
PCB* rrGetNextProcess(LinkedList* readyQueue) {
	return removeHead(readyQueue);
}


