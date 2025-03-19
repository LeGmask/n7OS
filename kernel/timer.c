#include <n7OS/timer.h>
#include <n7OS/console.h>
#include <stdio.h>

uint32_t timestamp = 0;

void init_timer()
{
	// Configure PIT
	outb(0x34, 0x43);
	outb(FREQ & 0xFF, 0x40);
	outb(FREQ >> 8, 0x40);

	// PIC configuration
	outb(inb(0x21) & 0xfe, 0x21); // Unmask IRQ0
}

void tick()
{
	timestamp++;

	if (timestamp % 1000 == 0)
	{
		update_clock(timestamp / 1000);
	}
	outb(0x20, 0x20); // EOI
}
