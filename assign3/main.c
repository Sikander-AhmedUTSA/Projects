#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "threading.h"
#include "linkedlist.h"
#include "fifo.h"
#include "sjf.h"
#include "pr.h"
#include "rr.h"

char* selectedAlgorithm = NULL;
int quantumTime = 0; // Only for RR
PCB* (*getNextProcess)(LinkedList*); // Global function for getting next process
int globalPID = 0;

double totalCPUBusyTime = 0.0;
int processesCompleted = 0.0;
double totalTurnaroundTime = 0.0;
double totalWaitingTime = 0.0;
double startTime = 0.0;
double endTime = 0.0;



void parseArguments(int argc, char* argv[], char** inputFile) { // Function to parse command line arguments and set global variables
	if (argc < 5) {
		fprintf(stderr, "Usage: %s -alg [FIFO|SJF|PR|RR] [-quantum <ms>] -input <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	*inputFile = NULL;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-alg") == 0 && i + 1 < argc) {
			selectedAlgorithm = argv[i + 1];
			i++;
		} else if (strcmp(argv[i], "-quantum") == 0 && i + 1 < argc) {
			quantumTime = atoi(argv[i + 1]);
			i++;
		} else if (strcmp(argv[i], "-input") == 0 && i + 1 < argc) {
			*inputFile = argv[i + 1];
		}

	}

	if (!selectedAlgorithm || !(*inputFile)) {
		fprintf(stderr, "Error: Missing algorithm or input file parameter\n");
		exit(EXIT_FAILURE);
	}

	// Set the function pointer based on the algorithm
	if (strcmp(selectedAlgorithm, "FIFO") == 0) {
		getNextProcess = fifoGetNextProcess;
	} else if (strcmp(selectedAlgorithm, "SJF") == 0) {
		getNextProcess = sjfGetNextProcess; 
	} else if (strcmp(selectedAlgorithm, "PR") == 0) {
		getNextProcess = prGetNextProcess;
	} else if (strcmp(selectedAlgorithm, "RR") == 0) {
		if (quantumTime <= 0) {
			fprintf(stderr, "Error, RR algorithm needs non-zero quantum time\n");
			exit(EXIT_FAILURE);
		} 
		getNextProcess = fifoGetNextProcess;
	} else {
		fprintf(stderr, "Error: Unknown algorithm %s\n", selectedAlgorithm);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char* argv[]) {
	char* inputFile = NULL;
	parseArguments(argc, argv, &inputFile);

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	startTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000.0;

	readyQueue = createList();
	ioQueue = createList();
	sem_init(&readyQueueSem, 0, 0);
	sem_init(&ioSem, 0, 0);

	pthread_t fileThread, cpuThread, ioThread;
	pthread_create(&fileThread, NULL, fileReadThread, inputFile);
	pthread_create(&cpuThread, NULL, cpuSchedulerThread, NULL);
	pthread_create(&ioThread, NULL, ioSystemThread, NULL);

	pthread_join(fileThread, NULL);
	pthread_join(cpuThread, NULL);
	pthread_join(ioThread, NULL);

	struct timespec ts2;
	clock_gettime(CLOCK_MONOTONIC, &ts2);
	endTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000.0;

	double totalTime = endTime - startTime;
	double cpuUtilization = (totalCPUBusyTime / totalTime) * 100.0;
	double avgTurnaroundTime = processesCompleted > 0 ? totalTurnaroundTime / processesCompleted : 0;
	double avgWaitingTime = processesCompleted > 0 ? totalWaitingTime / processesCompleted : 0;
	double throughput = totalTime > 0 ? processesCompleted / (totalTime /1000.0) : 0;

	printf("Simulation done! (I hope anyways)\n");
	printf("File name %s\n", inputFile);
	printf("CPU Scheduling Algorithm %s", selectedAlgorithm);
	if (strcmp(selectedAlgorithm, "RR") == 0)
		printf("Quantum time %d\n", quantumTime);
	printf("CPU Utilization %.2f \n", cpuUtilization);
	printf("Throughput %.2f\n", throughput);
	printf("Average Turnaround Time %.2f ms\n", avgTurnaroundTime);
	printf("Average Waiting Time %.2f ms\n", avgWaitingTime);




	freeList(readyQueue);
	freeList(ioQueue);
	sem_destroy(&readyQueueSem);
	sem_destroy(&ioSem);
	pthread_mutex_destroy(&readyQueueMutex);
	pthread_mutex_destroy(&ioQueueMutex);

	return 0;
}
