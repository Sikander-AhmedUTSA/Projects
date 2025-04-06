#ifndef PCB_H
#define PCB_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct PCB {
	int pid;
	int priority;
	int numCPUBurst, numIOBurst;
	int *CPUBurst, *IOBurst; // To create dynamic arrays (double linked list )to store burst
	int cpuIndex, ioIndex;
	struct PCB *prev, *next;

	double readyEntryTime; // Timestamp when PCB entered ready queue
	struct timespec ts_begin;
	struct timespec ts_end; // When the process enters and leaves the system
	double waiting_time, turnaround_time; // Calculated waiting time and total turnaournd time

} PCB;

PCB* createPCB(int pid, int priority, int numCPUBurst, int numIOBurst, int *CPUBurst, int *IOBurst, int cpuIndex, int ioIndex);
void initializePCB(PCB *pcb, int pid, int priority, int numCPUBurst, int numIOBurst, int *CPUBurst, int *IOBurst);
void start_pcb_timer(PCB *pcb);
void end_pcb_timer(PCB *pcb);
void printPCB(PCB *pcb);
void freePCB(PCB *pcb);


#endif
