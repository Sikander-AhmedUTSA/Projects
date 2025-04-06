#ifndef THREADING_H
#define THREADING_H

#include <pthread.h>
#include <semaphore.h>
#include "pcb.h"
#include "linkedlist.h"

// Global queues
extern pthread_mutex_t readyQueueMutex;
extern pthread_mutex_t ioQueueMutex;
extern sem_t readyQueueSem;
extern LinkedList* readyQueue;
extern LinkedList* ioQueue;

extern sem_t cpuSem;
extern sem_t ioSem;
//pthread_mutex_t cpuMutex;
//pthread_mutex_t ioMutex;




double currentTimeInMs();
// Functions for the threads
void* fileReadThread(void *arg);
void* cpuSchedulerThread(void *arg);
void* ioSystemThread(void *arg);



#endif
