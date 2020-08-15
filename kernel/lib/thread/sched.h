#ifndef _schedh
#define _schedh
#include "process.h"

/*
Structure of the scheduler

1) Create a process
2) Add to readyq when ready to execute
3) Once the current runningq is empty, processes in the readyq are moved to the runningq
4) Once a swap event happens, the current running process is saved and moved to the readyq, and the next process is loaded from the runningq

*/

#define SCHED_TICK 10

typedef struct Queue_node
{
	Process* data;
	struct Queue_node* next;
	struct Queue_node* prev;
} Queue_node;

typedef struct Queue
{
	Queue_node* head;
	Queue_node* tail;
} Queue;

void sched_init();
void doScheduleEvent(unsigned int* runningContext);
void addProcess(Process* process);
void removeProcess(Process* process);
Process* getCurrentProcess();

void enableScheduling();
void disableScheduling();

char getSchedulingStatus();

#endif