#ifndef _timerh
#define _timerh

#define TICKS_PER_SECOND 1000

void timer_init_timer();

unsigned int time_getsystime();
unsigned long time_getsysticks();

unsigned long long time_getCMOSTime();

void timer_wait(unsigned long ticks);
void timer_wait_ms(unsigned long ms);

void timer_timeinterrupt();

#endif