#ifndef RR_H
#define RR_H

#include "pcb.h"
#include "linkedlist.h"

//Use the queantam time given in the input
//And each "Process" will sleep for that long
//If CPU is greater than quantom time, then subtract it from it and do it?
//Look at video to get better ideas

PCB* rrGetNextProcess(LinkedList* readyQueue);

#endif
