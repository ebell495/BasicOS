#ifndef SCHED_H
#define SCHED_H
#include "process.h"
#include "../util/timer.h"

typedef struct QueueNode
{
	Process* process;
	struct QueueNode* next;
	struct QueueNode* prev;

	unsigned char state;
} QueueNode;

typedef struct RunQueue
{
	QueueNode* head;
	QueueNode* tail;
	unsigned int size;
} RunQueue;

void init_sched();
void enablePreempt();
void disablePreempt();

void doSwapInterrupt();
void queueProcess(Process* process);
void dequeueProcess(Process* process);

#endif