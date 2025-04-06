#ifndef FIFO_H
#define FIFO_H

#include "pcb.h"
#include "linkedlist.h"

// For this, create a queue for it, including ready queue and maybe Io queue as well?
// Create whatever algorithms for it

// The IO Queue will always be FIFO, so implement that here and do everything from here for IO
PCB* fifoGetNextProcess(LinkedList* readyQueue);


#endif
