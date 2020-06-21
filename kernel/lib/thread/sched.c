#include "sched.h"
#include "../util/memlib.h"
#include "../drv/hwio.h"

#define TIME_QUANTUM_TO_SWAP 10

struct RunQueue runQueue;
struct QueueNode emptyProcessNode;
struct Process* currentRunningProcess;
unsigned char doPreempt = 0;
unsigned int lastSwapTick = 0;

void defaultLoopProcess()
{
	unsigned long long lastTick = 0;
	while(1)
	{
		if(lastTick + 2579 < time_getsysticks())
		{
			lastTick = time_getsysticks();
			p_serial_printf("EMPTY %i\n", lastTick);
		}
		else
		{
			__asm__("int $80");
		}
	}
}

struct Process* emptyProcess;

void init_sched()
{
	emptyProcess = createProcess(defaultLoopProcess, "empty");
	p_serial_printf("EP address: %xi\n", emptyProcess);
	emptyProcessNode.process = emptyProcess;
	emptyProcessNode.state = STOPPED_STATE;

	emptyProcessNode.next = 0;
	emptyProcessNode.prev = 0;

	// runQueue.head = &emptyProcessNode;
	// runQueue.tail = &emptyProcessNode;
	// runQueue.size = 1;

	//addToRQ(emptyProcessNode);

	p_serial_printf("EPN address: %xi\n", &emptyProcessNode);

}

void injectProcess(QueueNode* queueNode)
{
	int scanCode = 0xDEAD;
	unsigned int* endPointer = &scanCode;
	unsigned int tmp = &scanCode;
	unsigned int offset = 0;
	unsigned char done = 0;

	__asm__("movl %%ebp, %0" : "=r" (endPointer) : );

	//p_serial_printf("%xi\n", endPointer);

	for(int i = 0; i < 64; i++)
	{
		//p_serial_printf("%xi: %xi\n", (unsigned int*)((&scanCode + i)), *(unsigned int*)((&scanCode + i)));
	}

	//p_serial_printf("\n\n\n");

	while(done < 3)
	{
		//p_serial_printf("%xi, %xi, %xi\n", endPointer, *endPointer, tmp);
		if(*endPointer - tmp > 12*4 && done == 2)
		{
			offset = ((unsigned int)endPointer) - ((unsigned int)(&scanCode)) + 8;
		}
		else
		{
			tmp = endPointer;
			endPointer = *endPointer;
		}
		done++;
	}
	//p_serial_printf("\n%xi, %xi, %xi, %xi\n", endPointer, tmp, offset, (unsigned int*)((&scanCode + (offset / 4))));
	//p_serial_printf("\n\n\n");

	//p_serial_printf("id : %i, Name : %s\n", queueNode->process->id, queueNode->process->procName);

	if(currentRunningProcess != 0)
	{
		memcpy(&(currentRunningProcess->savedState.registers), (unsigned int*)((&scanCode + (offset / 4))), 11*4);
	}

	memcpy((unsigned int*)((&scanCode + (offset / 4))), &(queueNode->process->savedState.registers), 11*4);

	for(int i = 0; i < 64; i++)
	{
		//p_serial_printf("%xi: %xi\n", (unsigned int*)((&scanCode + i)), *(unsigned int*)((&scanCode + i)));
	}

	currentRunningProcess->lastRunTick = time_getsysticks();

	currentRunningProcess = queueNode->process;
}

QueueNode* removeFromRQ()
{
	if(runQueue.tail == 0)
	{
		return 0;
	}
	else if(runQueue.tail == runQueue.head)
	{
		QueueNode* ret = runQueue.tail;
		runQueue.tail = 0;
		runQueue.head = 0;
		runQueue.size = 0;
		return ret;
	}
	else
	{
		QueueNode* ret = runQueue.tail;
		runQueue.tail = runQueue.tail->prev;
		runQueue.tail->next = 0;
		runQueue.size--;

		ret->prev = 0;
		return ret;
	}
}

void addToRQ(QueueNode* node)
{
	node->next = runQueue.head;
	node->prev = 0;

	runQueue.size++;

	if(runQueue.head == 0)
	{
		runQueue.head = node;
		runQueue.tail = node;
	}
	else
	{
		runQueue.head->prev = node;
		runQueue.head = node;
	}
}

QueueNode* cycleRQ()
{
	QueueNode* removed = removeFromRQ();
	if(removed == 0)
		return 0;

	addToRQ(removed);
	return removed;
}

void enablePreempt()
{
	doPreempt = 1;
}

void disablePreempt()
{
	doPreempt = 0;
}

void doSwapInterrupt()
{
	if(doPreempt)
	{	
		if(lastSwapTick + TIME_QUANTUM_TO_SWAP > time_getsysticks())
			return;
		lastSwapTick = time_getsysticks();

		if(runQueue.tail == 0)
		{
			//p_serial_printf("Sched Error: Empty Queue\n");
			injectProcess(&emptyProcessNode);
		}
		else
		{
			QueueNode* toRun = cycleRQ();
			injectProcess(toRun);
		}
	}
}

void queueProcess(struct Process* process)
{
	QueueNode* toAdd = kmalloc(sizeof(QueueNode));
	toAdd->process = process;
	addToRQ(toAdd);
}

void dequeueProcess(struct Process* process)
{
	QueueNode* ptr = runQueue.head;

	while(ptr != 0)
	{
		if(ptr->process == process)
		{
			if(ptr == runQueue.head)
			{
				runQueue.head = ptr->next;
				if(runQueue.head != 0)
				{
					runQueue.head->prev = 0;
				}
				
				runQueue.size--;
				kfree(ptr);
				return;
			}
			else if(ptr == runQueue.tail)
			{
				kfree(removeFromRQ());
				return;
			}
			else
			{
				ptr->next->prev = ptr->prev;
				ptr->prev->next = ptr->next;

				runQueue.size--;
				kfree(ptr);
				return;
			}
		}

		ptr = ptr->next;
	}
}
