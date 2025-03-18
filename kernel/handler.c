#include <inttypes.h>
#include <n7OS/irq.h>
#include <stdio.h>

extern void handler_IT_50();

void init_IT_50()
{
	init_irq_entry(50, (uint32_t)handler_IT_50);
}

void handler_IT_50_C()
{
	printf("interruption->$50: catched !\n");
}
