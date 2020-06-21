#include "process.h"
#include "../util/memlib.h"

unsigned int idCounter = 0;

struct Process* createProcess(void (*entryFunc)(void), char* name)
{
	struct Process* process = kmalloc(sizeof(struct Process));
	process->id = idCounter++;
	process->savedState.registers.eip = (unsigned int)entryFunc;
	process->entryPoint = (unsigned int)entryFunc;

	//Set the stack to some place in memory
	process->savedState.savedStack = kmalloc(PROCESS_STACK_SIZE);
	process->savedState.registers.ebp = process->savedState.savedStack;
	process->savedState.registers.esp = process->savedState.savedStack;
	process->savedState.registers.cs = 0x8;
	process->savedState.registers.eflags = 0x206;

	int nCount = 0;
	while(name[nCount] && nCount < 15)
	{
		process->procName[nCount] = name[nCount];
		nCount++;
	}

	return process;
}