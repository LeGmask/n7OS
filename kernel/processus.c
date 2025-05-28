#include <n7OS/cpu.h>
#include <n7OS/processus.h>
#include <malloc.h>

uint32_t free_process_table_bitmap[NB_PROC_MAX / 32];
process_t process_table[NB_PROC_MAX];

extern void ctx_sw(void *ctx_old, void *ctx_nex);
extern void idle();

/**
 * Find a free process ID.
 * @return The process ID if a free slot is found, otherwise returns NB_PROC_MAX.
 */
uint32_t findPid()
{
	for (uint32_t process_idx = 0; process_idx < NB_PROC_MAX; process_idx++)
	{
		uint32_t index = process_idx / 32;
		uint32_t offset = process_idx % 32;

		if (!(free_process_table_bitmap[index] & (1U << offset)))
		{
			free_process_table_bitmap[index] |= (1U << offset);
			return process_idx;
		}
	}
	return NB_PROC_MAX; // Return an invalid PID if no free slot is found
}

/**
 * Free a process ID.
 * @param process_idx The process ID to free.
 */
void freePid(uint32_t process_idx)
{
	uint32_t offset = process_idx % 32;
	free_process_table_bitmap[process_idx / 32] &= ~(1 << offset);
}

/**
 * Check if a process ID is valid and in use.
 * @param process_idx The process ID to check.
 * @return 1 if the process ID is valid and in use, otherwise returns 0.
 */
uint32_t isProcess(uint32_t process_idx)
{
	uint32_t offset = process_idx % 32;
	return (free_process_table_bitmap[process_idx / 32] & (1 << offset)) >> offset == 1;
}

/**
 * Spawn a new process.
 * @param name The name of the process.
 * @param main The main function of the process.
 * @return The process ID of the newly spawned process.
 */
uint32_t spawnProcess(char *name, void *main)
{
	uint32_t pid = findPid();
	process_table[pid].name = name;
	process_table[pid].pid = pid;
	process_table[pid].state = PRET;
	process_table[pid].stack = (uint32_t *)malloc(sizeof(uint32_t) * STACK_SIZE);

	for (int i = 0; i < 5; i++)
		process_table[pid].regs[i] = 0;

	process_table[pid].stack[STACK_SIZE - 1] = (uint32_t)main;
	process_table[pid].regs[ESP] = (uint32_t)&process_table[pid].stack[STACK_SIZE - 1];
	;

	return pid;
}

/**
 * Terminate a process.
 * @param pid The process ID of the process to terminate.
 */
void terminateProcess(PROCESS_ID pid)
{
	if (pid == 0)
		return; // idle process should not be terminated

	if (!isProcess(pid))
		return;

	free((void *)process_table[pid].stack);
	freePid(pid);
}

/**
 * Stop a process.
 * @param pid The process ID of the process to stop.
 */
void stopProcess(PROCESS_ID pid)
{
	process_table[pid].state = PRET;
}

/**
 * Block a process.
 * @param pid The process ID of the process to block.
 */
void blockProcess(PROCESS_ID pid)
{
	if (!isProcess(pid) && process_table[pid].state != PRET)
		return;

	process_table[pid].state = BLOQUE;
}

/**
 * Unblock a process.
 * @param pid The process ID of the process to unblock.
 */
void unblockProcess(PROCESS_ID pid)
{
	if (!isProcess(pid) && process_table[pid].state != BLOQUE)
		return;

	process_table[pid].state = PRET;
}

/**
 * Get the current process ID.
 * @return The process ID of the currently running process.
 */
uint32_t get_current_pid()
{
	for (uint32_t idx = 0; idx < NB_PROC_MAX; idx++)
	{
		if (isProcess(idx) && process_table[idx].state == ELU)
			return idx;
	}
}

/**
 * Initialize the process table and the free process bitmap.
 */
void init_process()
{
	for (uint32_t idx = 0; idx < NB_PROC_MAX / 32; idx++)
		free_process_table_bitmap[idx] = 0;
	process_table[spawnProcess("idle", *idle)].state = ELU;
}

/**
 * Schedule the next process to run.
 * This function finds the next process in the ready state and switches to it.
 */
void schedule()
{
	uint32_t pid = get_current_pid();
	for (int32_t idx = 0; idx < NB_PROC_MAX; idx++)
	{
		uint32_t new_pid = (pid + idx) % NB_PROC_MAX;
		if (isProcess(new_pid) && process_table[new_pid].state == PRET)
		{
			process_table[pid].state = PRET;
			process_table[new_pid].state = ELU;

			// printf("Switching to process %d : %s\n", new_pid, process_table[new_pid].name);
			sti();
			ctx_sw(process_table[pid].regs, process_table[new_pid].regs);
			return;
		}
	}
}

/**
 * Print the list of processes.
 * This function displays the process ID, name, and state of each process in the process table.
 */
void ps()
{
	printf("%-5s %-16s %-8s\n", "PID", "Name", "State");
	for (uint32_t idx = 0; idx < NB_PROC_MAX; idx++)
	{
		if (isProcess(idx))
		{
			printf("%-5d %-16s ", process_table[idx].pid, process_table[idx].name);
			switch (process_table[idx].state)
			{
			case PRET:
				printf("%-8s\n", "PRET");
				break;
			case ELU:
				printf("%-8s\n", "ELU");
				break;
			case BLOQUE:
				printf("%-8s\n", "BLOQUE");
				break;
			default:
				printf("%-8s\n", "UNKNOWN");
				break;
			}
		}
	}
}
