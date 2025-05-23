#include <n7OS/cpu.h>
#include <inttypes.h>
#include <n7OS/processor_structs.h>
#include <n7OS/console.h>
#include <n7OS/mem.h>
#include <n7OS/paging.h>
#include <n7OS/kheap.h>
#include <n7OS/it_handlers.h>
#include <n7OS/timer.h>
#include <n7OS/sys.h>
#include <unistd.h>
#include <stdio.h>
#include <n7OS/processus.h>

extern void idle();

void test2()
{
	while (1)
	{
		printf("Hello world 2\n");
	}
}

void test()
{
	while (1)
	{
		printf("Hello world 1\n");
	}
}

void kernel_start(void)
{
	init_console();
	init_kheap();
	initialise_paging();

	init_IT();
	sti();
	init_timer();

	init_syscall();

	__asm__("int $50");

	if (example() == 1)
	{
		printf("Appel systeme example ok \n");
	}

	// shutdown(1);

	// print_mem();

	init_process();
	spawnProcess("test", test);
	spawnProcess("test2", test2);

	// on ne doit jamais sortir de kernel_start
	idle();
}
