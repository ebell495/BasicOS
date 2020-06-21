#ifndef PROCESS_H
#define PROCESS_H

#define RUNNING_STATE 1
#define PAUSED_STATE 2
#define STOPPED_STATE 0

#define PROCESS_STACK_SIZE 4096

struct ContextInfo
{
	unsigned char* savedStack;
	struct Registers
	{
		unsigned int edi;
		unsigned int esi;
		unsigned int ebp;
		unsigned int esp;
		unsigned int ebx;
		unsigned int edx;
		unsigned int ecx;
		unsigned int eax;
		unsigned int eip;
		unsigned int cs;
		unsigned int eflags;
	} registers;
};

typedef struct Process
{
	unsigned int id;
	unsigned int runningTicks;
	unsigned int lastRunTick;
	unsigned int entryPoint;
	unsigned char procName[16];
	struct ContextInfo savedState;
} Process;

Process* createProcess(void (*entryFunc)(void), char* name);


#endif