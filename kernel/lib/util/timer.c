#include "timer.h"
#include "../drv/hwio.h"

unsigned long systicks = 0;

void timer_init_timer()
{
	int divisor = 1193180 / TICKS_PER_SECOND;
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

unsigned long long time_getCMOSTime()
{
	pbyteout(0x70, (1 << 7) | (0x00));
	unsigned char sec = pbytein(0x71);

	pbyteout(0x70, (1 << 7) | (0x02));
	unsigned char min = pbytein(0x71);

	pbyteout(0x70, (1 << 7) | (0x04));
	unsigned char hours = pbytein(0x71);

	pbyteout(0x70, (1 << 7) | (0x07));
	unsigned char days = pbytein(0x71);

	pbyteout(0x70, (1 << 7) | (0x08));
	unsigned char months = pbytein(0x71);

	pbyteout(0x70, (1 << 7) | (0x09));
	unsigned char years = pbytein(0x71);

	return 31557600*years + 2629800*months + 87660*days + 3652*hours + 60*min + sec;
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