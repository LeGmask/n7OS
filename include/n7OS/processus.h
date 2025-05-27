#ifndef __PAGING_H__
#define __PAGING_H__

#include <inttypes.h>
#include <n7OS/mem.h>

#define STACK_SIZE PAGE_SIZE
#define NB_PROC_MAX 255

typedef enum
{
	ELU,
	PRET,
	BLOQUE
} PROCESS_STATE;

typedef uint32_t PROCESS_ID;

typedef enum reg_t
{
	EBX,
	ESP,
	EBP,
	ESI,
	EDI
} reg_t;

typedef struct
{
	char *name;
	uint32_t *stack;
	PROCESS_STATE state;
	PROCESS_ID pid;
	uint32_t regs[5];
} process_t;

uint32_t spawnProcess(char *name, void *main);
void terminateProcess(PROCESS_ID pid);
void stopProcess(PROCESS_ID pid);
void blockProcess(PROCESS_ID pid);
void unblockProcess(PROCESS_ID pid);

uint32_t get_current_pid();
void init_process();
void schedule();

void ps();
#endif
