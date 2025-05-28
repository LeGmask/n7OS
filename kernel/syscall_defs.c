#include <n7OS/syscall_defs.h>

fn_ptr syscall_table[NB_SYSCALL];

/**
 * Add a syscall to the syscall table.
 * @param num The syscall number.
 * @param function The function pointer to the syscall handler.
 */
void add_syscall(int num, fn_ptr function)
{
	if (num < NB_SYSCALL)
	{
		syscall_table[num] = function;
	}
}
