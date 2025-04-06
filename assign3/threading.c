#include "threading.h"
//#include "pcb.h"
//#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>


// Thread identifiers potentially needed
//static pthread_t fileThread, cpuThread, ioThread;

// Declare global variables here
pthread_mutex_t readyQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ioQueueMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t readyQueueSem;
LinkedList* readyQueue;
LinkedList* ioQueue;

pthread_mutex_t cpuMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ioMutex= PTHREAD_MUTEX_INITIALIZER;
sem_t cpuSem;
sem_t ioSem;
extern PCB** (*getNextOricess)(LinkedList*);


// Global flags needed to end
volatile int fileReadDone = 0;
volatile int cpuBusy = 0;
volatile int ioBusy = 0;

extern int globalPID;
extern char* selectedAlgorithm;
extern int quantumTime;
extern PCB* (*getNextProcess)(LinkedList*);

extern double totalCPUBusyTime;
extern int processesCompleted;
extern double totalTurnaroundTime;
extern double totalWaitingTime;


double currentTimeInMs() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

void* fileReadThread(void* arg) { // Will take in an input file as a string and read through it

	char *filename = (char*) arg; // Pointer to an array of char, so should be fine like this
	FILE* file = fopen(filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[256]; // Parsing through the file line by line
	while (fgets(line, sizeof(line), file)) {
		char* token = strtok(line, " "); // Seperating by spaces
		if (strcmp(token, "proc") == 0) { // If it is proc
						  // Read the priority and the number of bursts
			int priority = atoi(strtok(NULL, " ")); // Null means to continue tokenizing, as in keep going (for my reference really)
			int numBursts = atoi(strtok(NULL, " "));

			//Allocate memory for bursts
			int *cpuBursts = malloc(sizeof(int) * ((numBursts + 1) / 2));
			int *ioBursts = malloc(sizeof(int) * (numBursts / 2));
			// Putting the 
			for (int i = 0; i < numBursts; i++) {
				int burst = atoi(strtok(NULL, " "));
				if (i % 2 == 0) // Alternating
					cpuBursts[i / 2] = burst;
				else
					ioBursts[i / 2] = burst;
			}


			// Create PCB
			PCB *pcb = createPCB(globalPID++ ,priority, (numBursts + 1) / 2, numBursts / 2, cpuBursts, ioBursts, 0, 0);

			// Insert into ready Queue
			pthread_mutex_lock(&readyQueueMutex);
			insertTail(readyQueue, pcb);
			pthread_mutex_unlock(&readyQueueMutex);

			// Signal CPU Scheduler to WAKE UP WAKE UP WAKE UPPPP
			sem_post(&readyQueueSem);
		}
		else if (strcmp(token, "sleep") == 0) { // Zzz Zzz Zzz moment
			int sleepTime = atoi(strtok("NULL", " "));
			usleep(sleepTime * 1000); // Convert to ms 
		}
		else if (strcmp(token, "stop") == 0) { // End of file
			break;
		}
	}

	fclose(file);
}

void* ioSystemThread(void *arg) {
	while (1) {
		// Wait until a process is in the IO Queue
		sem_wait(&ioSem);
		

		// Lock the IO queue to retrieve a process
		pthread_mutex_lock(&ioQueueMutex);
		PCB *pcb = removeHead(ioQueue);
		pthread_mutex_unlock(&ioQueueMutex);
		ioBusy = 1;

		// Simulation time
		int ioBurstTime = pcb->IOBurst[pcb->ioIndex];
		printf("Processing IO with priority %d\n", pcb->priority); // For testing purposes

		usleep(ioBurstTime * 1000);

		//Move to next IO burst
		pcb->ioIndex++;

		// Put the process back into the reaady queue
		pthread_mutex_lock(&readyQueueMutex);
		insertTail(readyQueue, pcb);
		pthread_mutex_unlock(&readyQueueMutex);

		//Signal CPU scheduler that a process is ready to go
		sem_post(&readyQueueSem);
		ioBusy = 0;
	}
	
}

void* cpuSchedulerThread(void* arg) {
	while (1) {
		// Condition check to make sure it is empty
		pthread_mutex_lock(&readyQueueMutex);
		int empty = isEmpty(readyQueue);
		pthread_mutex_unlock(&readyQueueMutex);

		if (fileReadDone && empty && !cpuBusy && !ioBusy) {
			break;
		}


		sem_wait(&readyQueueSem);

		double now = currentTimeInMs();

		//Get the process
		pthread_mutex_lock(&readyQueueMutex);
		PCB *pcb = removeHead(readyQueue);
		pthread_mutex_unlock(&readyQueueMutex);

		double waitTime = now - pcb->readyEntryTime;
		pcb->waiting_time += waitTime;
		totalWaitingTime += waitTime;
		cpuBusy = 1;



		//Get the burst time
		int cpuBurstTime = pcb->CPUBurst[pcb->cpuIndex];
		if (strcmp(selectedAlgorithm, "RR") == 0 &&  cpuBurstTime > quantumTime) // For Round Robin Algorithm
			cpuBurstTime = quantumTime;


		printf("Running process with priority %d for %d ms\n", pcb->priority, cpuBurstTime); // For testing Purposes
		usleep(cpuBurstTime * 1000);
		totalCPUBusyTime += cpuBurstTime;

		if(strcmp(selectedAlgorithm, "RR") == 0) {
			if (pcb->CPUBurst[pcb->cpuIndex] > quantumTime) {
				pcb->CPUBurst[pcb->cpuIndex] -= quantumTime;
				//Put at tail after making the time equal to quantum time
				pthread_mutex_lock(&readyQueueMutex);
				insertTail(readyQueue, pcb);
				pthread_mutex_unlock(&readyQueueMutex);
				sem_post(&readyQueueSem);
			} else { // Less than quantum time
				pcb->cpuIndex++;
				if (pcb->cpuIndex < pcb->numCPUBurst) {	// Send to IO
					pthread_mutex_lock(&readyQueueMutex);
					insertTail(readyQueue, pcb);
					pthread_mutex_unlock(&readyQueueMutex);
					sem_post(&ioSem);
				} else { // Its OVER
					end_pcb_timer(pcb);
					printf("PCB (PID: %d) finished Execution time. Turnaround Time: %.2f ms\n", pcb->pid, pcb->turnaround_time);
					totalTurnaroundTime += pcb->turnaround_time;
					processesCompleted++;
					freePCB(pcb);
				}
			}
		} else { // Same as above, but if Algorithm is FIFO
			pcb->cpuIndex++;
			if (pcb->cpuIndex < pcb->numCPUBurst) {	// Send to IO
				pthread_mutex_lock(&readyQueueMutex);
				insertTail(readyQueue, pcb);
				pthread_mutex_unlock(&readyQueueMutex);
				sem_post(&ioSem);
			} else { // Its OVER
				end_pcb_timer(pcb);
				printf("PCB (PID: %d) finished Execution time. Turnaround Time: %.2f ms\n", pcb->pid, pcb->turnaround_time);
				totalTurnaroundTime += pcb->turnaround_time;
				processesCompleted++;
				freePCB(pcb);
			}

		}
	}
}
