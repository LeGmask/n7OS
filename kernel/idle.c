#include <stdio.h>
#include <n7OS/processus.h>
#include <n7OS/cpu.h>

void idle()
{
	printf("Idle\n");
	while (1)
	{
		hlt();
	}
}
