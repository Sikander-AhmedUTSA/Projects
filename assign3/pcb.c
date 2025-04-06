#include "pcb.h"

/* For reference sake
   typedef struct PCB {
   int pid;
   int priority;
   int numCPUBurst, numIOBurst;
   int *CPUBurst, *IOBurst; // To create dynamic arrays (double linked list )to store burst
   int cpuIndex, ioIndex;
   struct PCB *prev, *next;
   double waiting_time, turnaround_time;

   struct timespec ts_begin, timespec ts_end; // When the process enters and leaves the system
   */


PCB* createPCB(int pid, int priority, int numCPUBurst, int numIOBurst, int *CPUBurst, int *IOBurst, int cpuIndex, int ioIndex) { // Creates the PCB and initalizes the values inside of it
	PCB *pcb = (PCB*)malloc(sizeof(PCB));
	if (!pcb) {
		perror("Failed to allocate memory for PCB");
		return NULL;
	}
	initializePCB(pcb, pid, priority, numCPUBurst, numIOBurst, CPUBurst, IOBurst);
}

void initializePCB(PCB *pcb, int pid, int priority, int numCPUBurst, int numIOBurst, int *CPUBurst, int *IOBurst){ // Initializing everything 
	pcb->pid = pid;
	pcb->priority = priority;
	pcb->numCPUBurst = numCPUBurst;
	pcb->numIOBurst = numIOBurst;
	pcb->CPUBurst = CPUBurst;
	pcb->IOBurst = IOBurst;
	
	pcb->cpuIndex = 0;
	pcb->ioIndex = 0;
	pcb->waiting_time = 0;
	pcb->turnaround_time = 0;
	pcb->readyEntryTime = 0;

	// Mallocing the arrays needed and then setting the values inside of it
	pcb->CPUBurst = (int*)malloc(numCPUBurst * sizeof(int));
	pcb->IOBurst = (int*)malloc(numIOBurst * sizeof(int));

	if (!pcb->CPUBurst || !pcb->IOBurst) {
		perror("Failed to allocate memeory for burst arrays");
		freePCB(pcb);
	}

	for (int i = 0; i < numCPUBurst; i++) 
		pcb->CPUBurst[i] = CPUBurst[i];

	for (int i = 0; i < numIOBurst; i++) 
		pcb->IOBurst[i] = IOBurst[i];

	pcb->prev = NULL;
	pcb->next = NULL;
}


void start_pcb_timer(PCB *pcb) { // Starts the timer
	if (pcb) 
		clock_gettime(CLOCK_MONOTONIC, &pcb->ts_begin);
}


void end_pcb_timer(PCB *pcb) { // Ends the PCB timer and calculates waiting and turnaround time
	if (pcb) {
		clock_gettime(CLOCK_MONOTONIC, &pcb->ts_end);
		pcb->turnaround_time = (pcb->ts_end.tv_sec - pcb->ts_begin.tv_sec) * 1000.0; 
		pcb->turnaround_time += (pcb->ts_end.tv_nsec - pcb->ts_begin.tv_nsec) / 1000000.0;
	}
}


void printPCB(PCB *pcb) { // Prints the PCB data
	int i;
	if (!pcb)
		return;
	printf("PCB PID: %d\n", pcb->pid);
	printf("PCB Priority: %d\n", pcb->priority);

	printf("CPU Bursts: ");
	for (int i = 0; i < pcb->numCPUBurst; i++)
		printf("%d ", pcb->CPUBurst[i]);
	printf("\nIO Bursts: ");
	for (int i = 0; i < pcb->numIOBurst; i++)
		printf("%d ", pcb->IOBurst[i]);

	printf("Waiting Time: %.2f ms\n", pcb->waiting_time);
	printf("Turnaround Time: %.2f ms\n", pcb->turnaround_time);
}

void freePCB(PCB *pcb) { // Frees the whole PCB
	if (!pcb)
		return;
	if (pcb->CPUBurst)
		free(pcb->CPUBurst);
	if (pcb->IOBurst)
		free(pcb->IOBurst);
	free(pcb);
}
