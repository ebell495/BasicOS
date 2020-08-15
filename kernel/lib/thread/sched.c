#include "sched.h"
#include "../util/memlib.h"
#include "../drv/hwio.h"

Queue waiting;
Queue running;
Queue kill;
Process* currentProcess;
char sched_status = 0;

unsigned char queue_verifyqueue(Queue* queue)
{
	int forwardSize = 0;
	int backwardSize = 0;

	Queue_node* node = queue->head;

	while(node != 0 && node != queue->tail)
	{
		forwardSize++;
		node = node->next;
	}

	if(node == queue->tail)
	{
		forwardSize++;
		while(node->next != 0)
		{
			forwardSize++;
			node = node->next;
		}
	}

	node = queue->tail;

	while(node != 0 && node != queue->head)
	{
		backwardSize++;
		node = node->prev;
	}

	if(node == queue->head)
	{
		backwardSize++;
		while(node->prev != 0)
		{
			backwardSize++;
			node = node->prev;
		}
	}

	return forwardSize - backwardSize;
}

void queue_enqueue(Queue* queue, Process* data)
{
	Queue_node* node = kmalloc(sizeof(Queue_node));
	node->data = data;
	node->next = queue->head;
	node->prev = 0;

	if(queue->head == 0)
	{
		queue->tail = node;
		queue->head = node;

		if(queue_verifyqueue(queue))
		{
			p_serial_printf("ERROR: queue_verifyqueue failed in 1) queue_enqueue\n");
		}
		return;
	}

	queue->head->prev = node;
	queue->head = node;

	if(queue_verifyqueue(queue))
	{
		p_serial_printf("ERROR: queue_verifyqueue failed in 2) queue_enqueue\n");
	}
}

Process* queue_dequeue(Queue* queue)
{
	Queue_node* node = queue->tail;

	if(node == 0)
		return 0;

	queue->tail = node->prev;
	if(queue->tail == 0)
	{
		queue->head = 0;
	}
	else
	{
		queue->tail->next = 0;
	}

	Process* ret = node->data;
	kfree(node);

	if(queue_verifyqueue(queue))
	{
		p_serial_printf("ERROR: queue_verifyqueue failed in 1) queue_dequeue\n");
	}

	return ret;
}

unsigned char queue_remove(Queue* queue, Process* process)
{
	Queue_node* node = queue->head;

	while(node != 0)
	{
		if(node->data == process)
		{
			if(node == queue->head)
			{
				queue->head = queue->head->next;
				if(queue->head != 0)
					queue->head->prev = 0;

				if(node == queue->tail)
				{
					queue->tail = node->prev;
					if(queue->tail != 0)
						queue->tail->next = 0;

					
				}

				if(queue_verifyqueue(queue))
				{
					p_serial_printf("ERROR: queue_verifyqueue failed in 1) queue_remove\n");
				}

				kfree(node);
				return 1;
			}
			else if(node == queue->tail)
			{
				queue->tail = node->prev;
				if(queue->tail != 0)
					queue->tail->next = 0;

				if(queue_verifyqueue(queue))
				{
					p_serial_printf("ERROR: queue_verifyqueue failed in 2) queue_remove\n");
				}	

				kfree(node);
				return 1;
			}
			else
			{
				node->next->prev = node->prev;
				node->prev->next = node->next;
				
				if(queue_verifyqueue(queue))
				{
					p_serial_printf("ERROR: queue_verifyqueue failed in 3) queue_remove\n");
				}

				kfree(node);
				return 1;
			}
		}
		node = node->next;
	}

	return 0;
}

int queue_size(Queue* queue)
{
	int size = 0;

	Queue_node* node = queue->head;

	while(node != 0)
	{
		node = node->next;
		size++;
	}

	return size;
}

void process_swap(Process* nextProcess, unsigned int* runningContext)
{
	//Swap out the old process
	p_serial_printf("%xi\n", *runningContext);
	if(currentProcess != 0)
	{
		p_serial_printf("DEBUG: Last process ID: %i ", currentProcess->id);

		// for(int i = 0; i < 16; i++)
		// {
		// 	p_serial_printf("%i: %xi\n", i, ((unsigned int*)(runningContext))[i]);
		// }

		memcpy(&(currentProcess->savedState.registers), runningContext, 16*4);
		p_serial_printf("EIP: %xi\n", currentProcess->savedState.registers.eip);

		// for(int i = 0; i < 16; i++)
		// {
		// 	p_serial_printf("%i: %xi\n", i, ((unsigned int*)(&currentProcess->savedState.registers))[i]);
		// }

		if(currentProcess->savedState.registers.eip <= 0x1500 || currentProcess->savedState.registers.eip >= 0x9500)
		{
			p_serial_printf("ERROR: Process %i with corrupt instruction pointer after saving. EIP was: %xi  Reinitializing process\n", currentProcess->id, currentProcess->savedState.registers.eip);
			currentProcess->savedState.registers.eip = currentProcess->entryPoint;
			currentProcess->savedState.registers.ebp = (unsigned int) currentProcess->savedState.savedStack - PROCESS_STACK_SIZE;
			currentProcess->savedState.registers.esp = (unsigned int) currentProcess->savedState.savedStack - PROCESS_STACK_SIZE;

			//Set the default code segment register to 0x08
			currentProcess->savedState.registers.cs = 0x8;
			currentProcess->savedState.registers.eflags = 0x293;
			if(currentProcess->savedState.registers.eip <= 0x1500 || currentProcess->savedState.registers.eip >= 0x9500)
			{
				p_serial_printf("ERROR: Process fully corrupt, killing...\n");
				currentProcess = 0;
			}
		}
	}

	currentProcess = nextProcess;

	if(currentProcess->savedState.registers.esp == currentProcess->savedState.registers.ebp)
		currentProcess->savedState.registers.esp -= 16*4;

	*runningContext = (currentProcess->savedState.registers.esp);
	//p_serial_printf("%xi\n", *runningContext);
	memcpy((unsigned int*)currentProcess->savedState.registers.esp-1, &(currentProcess->savedState.registers), 16*4);

	p_serial_printf("DEBUG: New process ID: %i EIP: %xi\n", currentProcess->id, currentProcess->savedState.registers.eip);

	if(currentProcess->savedState.registers.eip <= 0x1500 || currentProcess->savedState.registers.eip >= 0x9500)
	{
		p_serial_printf("ERROR: New process %i with corrupt instruction pointer. EIP was: %xi  Reinitializing process\n", currentProcess->id, currentProcess->savedState.registers.eip);
		currentProcess->savedState.registers.eip = currentProcess->entryPoint;
		currentProcess->savedState.registers.ebp = (unsigned int) currentProcess->savedState.savedStack - PROCESS_STACK_SIZE;
		currentProcess->savedState.registers.esp = (unsigned int) currentProcess->savedState.savedStack - PROCESS_STACK_SIZE;
		currentProcess->savedState.registers.cs = 0x8;
		currentProcess->savedState.registers.eflags = 0x293;
		memcpy((unsigned int*)currentProcess->savedState.registers.esp-1, &(currentProcess->savedState.registers), 16*4);
		if(currentProcess->savedState.registers.eip <= 0x1500 || currentProcess->savedState.registers.eip >= 0x9500)
		{
			p_serial_printf("ERROR: Process fully corrupt, killing...\n");
			doScheduleEvent(runningContext);	
		}
	}

	// for(int i = 0; i < 16; i++)
	// {
	// 	p_serial_printf("%i: %xi\n", i, ((unsigned int*)(currentProcess->savedState.registers.esp))[i]);
	// }

	if(currentProcess != process_getNullProcess() && currentProcess != 0)
		queue_enqueue(&waiting, currentProcess);
}

void sched_init()
{
	waiting.head = 0;
	waiting.tail = 0;
	running.head = 0;
	running.tail = 0;
	kill.head = 0;
	kill.tail = 0;
	currentProcess = 0;
	sched_status = 0;
}

void doScheduleEvent(unsigned int* runningContext)
{
	//No new process to run, add in the readyq
	if(queue_size(&running) == 0)
	{
		if(queue_size(&waiting) == 0)
		{
			process_swap(process_getNullProcess(), runningContext);
			return;
		}
		else
		{
			//Move all processes that are ready to run, to the running queue
			//If a processes was sleeping, and the time is still not up, then keep in the waiting queue
			//This is inefficient and instead should anticipate when it will be ready and queue processes that are almost done sleeping
			//TODO: add sleep functionality

			while(waiting.head != 0)
			{
				Process* toAdd = queue_dequeue(&waiting);
				queue_enqueue(&running, toAdd);
			}
		}
	}

	if(queue_size(&kill) > 0)
	{
		while(kill.head != 0)
		{
			Process* proc = queue_dequeue(&kill);
			p_serial_printf("DEBUG: Removing process %i, %s\n", proc->id, proc->procName);
			if(queue_remove(&running, proc) == 0)
			{
				if(queue_remove(&waiting, proc) == 0)
				{
					p_serial_printf("ERROR: No such process is running or waiting, Process pointer %xi\n", proc);
				}
			}

			if(!queue_verifyqueue(&running))
			{
				p_serial_printf("ERROR: Corrupt running queue\n");
			}
			if(!queue_verifyqueue(&waiting))
			{
				p_serial_printf("ERROR: Corrupt waiting queue\n");
			}
		}
	}

	Process* nextProcess = queue_dequeue(&running);

	process_swap(nextProcess, runningContext);
}

void addProcess(Process* process)
{
	queue_enqueue(&waiting, process);
}

void removeProcess(Process* process)
{
	queue_enqueue(&kill, process);
}

Process* getCurrentProcess()
{
	return currentProcess;
}

void enableScheduling()
{
	sched_status = 1;
}

void disableScheduling()
{
	sched_status = 0;
}

char getSchedulingStatus()
{
	return sched_status;
}