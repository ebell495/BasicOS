#ifndef _processh
#define _processh

#define PROCESS_STACK_SIZE 4096

struct ContextInfo
{
	unsigned char* savedStack;
	struct Registers
	{
		unsigned int esp;
		unsigned int ebp;
		unsigned int esi;
		unsigned int edi;
		unsigned int edx;
		unsigned int ecx;
		unsigned int ebx;
		unsigned int eax;
		unsigned int ds;
		unsigned int gs;
		unsigned int fs;
		unsigned int es;
		unsigned int ss;
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

Process* process_getNullProcess();

#endif