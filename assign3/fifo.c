#include "fifo.h"

PCB* fifoGetNextProcess(LinkedList* readyQueue) { // All that needs to be down is just remove the first one from the list. Easy peasy
	return removeHead(readyQueue);
}
