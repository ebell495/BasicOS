#ifndef _timerh
#define _timerh

#define TIMER_RELOAD_VALUE 597
#define TIMER_MILLISECONDS_PER_TICK TIMER_RELOAD_VALUE * 3000 / 3579545

void timer_init_timer();

unsigned int time_getsystime();
unsigned long long time_getsysticks();

unsigned long long time_getCMOSTime();

void timer_wait(unsigned long ticks);
void timer_wait_ms(unsigned long ms);

void timer_timeinterrupt();

#endif