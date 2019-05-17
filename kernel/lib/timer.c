#include "timer.h"
#include "hwio.h"

unsigned long systicks = 0;

void timer_init_timer()
{
	int divisor = 1193180 / 100;
	pbyteout(0x43, 0x36);             /* Set our command byte 0x36 */
    pbyteout(0x40, divisor & 0xFF);   			/* Set low byte of divisor */
    pbyteout(0x40, divisor >> 8);    			 /* Set high byte of divisor */
	systicks = 0;
}

unsigned int time_getsystime()
{
	return (unsigned int) (systicks / TICKS_PER_SECOND);
}

unsigned long time_getsysticks()
{
	return systicks;
}

void timer_timeinterrupt()
{
	systicks++;
}

void timer_wait(unsigned long ticks)
{
	unsigned long endTicks = systicks + ticks;
	
	while(systicks < endTicks)
	{}
}

void timer_wait_ms(unsigned long ms)
{
	timer_wait(ms);
}