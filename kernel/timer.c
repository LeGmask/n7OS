#include <n7OS/timer.h>
#include <n7OS/console.h>
#include <n7OS/processus.h>
#include <stdio.h>

uint32_t timestamp = 0;

void rtc_sync()
{
	outb(0x0, 0x70);
	unsigned char second = inb(0x71);
	outb(0x2, 0x70);
	unsigned char minute = inb(0x71);
	outb(0x4, 0x70);
	unsigned char hour = inb(0x71);

	// BCD to binary conversion
	second = (second & 0x0F) + ((second / 16) * 10);
	minute = (minute & 0x0F) + ((minute / 16) * 10);
	hour = (hour & 0x0F) + ((hour / 16) * 10);

	timestamp = (hour * 3600 + minute * 60 + second) * 1000;
	update_clock(timestamp / 1000); // Update clock
}

void init_timer()
{
	// Synchronize timestamp with RTC
	rtc_sync();

	// Configure PIT
	outb(0x34, 0x43);
	outb(FREQ & 0xFF, 0x40);
	outb(FREQ >> 8, 0x40);

	// PIC configuration
	outb(inb(0x21) & 0xfe, 0x21); // Unmask IRQ0
}

void tick()
{
	outb(0x20, 0x20); // EOI
	timestamp++;

	if (timestamp % 1000 == 0)
	{
		update_clock(timestamp / 1000);
	}

	if (timestamp % 10 == 0)
	{
		schedule();
	}
}
