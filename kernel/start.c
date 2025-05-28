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
#include <n7OS/keyboard.h>
#include <n7OS/shell.h>

extern void idle();

void test_process()
{
	while (1)
	{
		printf("Test process running...\n");
		for (volatile int i = 0; i < 1000000; i++)
			; // Busy wait
	}
}

void kernel_start(void)
{
	init_console();
	init_kheap();
	init_paging();
	init_keyboard();
	init_timer();

	init_IT();
	sti();

	init_syscall();

	__asm__("int $50");
	if (example() == 1)
		printf("Interrupt test: OK!\n");

	printf("Memory test: ");
	alloc_page_entry(0xa000FFF8, 1, 1);
	uint32_t *ptr = (uint32_t *)0xa000FFFc;
	uint32_t test = *ptr;
	test++;
	// print_mem();
	printf("OK!\n");

	// shutdown(1);

	init_process();
	spawnProcess("shell", shell);
	// spawnProcess("test", test_process);

	// on ne doit jamais sortir de kernel_start
	idle();
}
