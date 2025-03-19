#include <n7OS/it_handlers.h>
#include <n7OS/irq.h>
#include <n7OS/cpu.h>
#include <n7OS/timer.h>
#include <inttypes.h>
#include <stdio.h>

extern void handler_IT_32();
extern void handler_IT_50();

void init_IT()
{
	init_irq_entry(32, (uint32_t)handler_IT_32);
	init_irq_entry(50, (uint32_t)handler_IT_50);
}

void handler_IT_50_C()
{
	printf("interruption->$50: catched !\n");
}
