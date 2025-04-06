#ifndef SJF_H
#define SJF_H

#include "pcb.h"
#include "linkedlist.h"

//Same as other ones but algorithm will first check which job is shortest in the queue
//Select CPUBurst, and then put that into the linked list somehow lol

PCB* sjfGetNextProcess(LinkedList* readyQueue);

#endif
