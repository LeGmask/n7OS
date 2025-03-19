#ifndef __TIMER_H__
#define __TIMER_H__

#include <n7OS/cpu.h>
#include <inttypes.h>

#define CLOCK 1000 // 1kHz
#define FREQ_OSC 0x1234BD
#define FREQ FREQ_OSC / CLOCK

void init_timer();
void tick(); // handler_IT_32

#endif
