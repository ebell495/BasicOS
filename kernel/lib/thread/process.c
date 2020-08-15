#include "process.h"
#include "../util/memlib.h"
#include "sched.h"

unsigned int idCounter = 0;

//Null Process does nothing
void nullProcessFunc()
{
	while(1)
	{

	}
}

void procEntry(void (*procStart)(void))
{
	procStart();

	Process* currentProc = getCurrentProcess();
	removeProcess(currentProc);

	while(1)
	{

	}
}

Process* nullProcess;

Process* process_getNullProcess()
{
	if(nullProcess == 0)
	{
		nullProcess = createProcess(nullProcessFunc, "null");
	}

	return nullProcess;
}

Process* createProcess(void (*entryFunc)(void), char* name)
{
	struct Process* process = kmalloc(sizeof(struct Process));
	process->id = idCounter++;
	process->savedState.registers.eip = (unsigned int)entryFunc;
	process->entryPoint = (unsigned int)entryFunc;

	//Set the stack to some place in memory
	process->savedState.savedStack = (unsigned char*) kmalloc(PROCESS_STACK_SIZE);
	process->savedState.registers.ebp = (unsigned int) process->savedState.savedStack + PROCESS_STACK_SIZE;
	process->savedState.registers.esp = (unsigned int) process->savedState.savedStack + PROCESS_STACK_SIZE;
	process->savedState.registers.ds = 0x10;
	process->savedState.registers.es = 0x10;
	process->savedState.registers.ss = 0x10;
	process->savedState.registers.gs = 0x10;
	process->savedState.registers.fs = 0x10;

	//Set the default code segment register to 0x08
	process->savedState.registers.cs = 0x8;
	process->savedState.registers.eflags = 0x293;

	//Copies upto 15 bytes of the name
	int nCount = 0;
	while(name[nCount] && nCount < 15)
	{
		process->procName[nCount] = name[nCount];
		nCount++;
	}

	return process;
}