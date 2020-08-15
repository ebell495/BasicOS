#include "timer.h"
#include "../drv/hwio.h"

unsigned long long systicks = 0;

void timer_init_timer()
{
	pbyteout(0x43, 0b00110110);
	//The divisor is 0x0255 which is 597 in decimal
	//This yields about a tick every .5ms             
    pbyteout(0x40, 0x55);   			/* Set low byte of divisor */
    pbyteout(0x40, 0x02);    			 /* Set high byte of divisor */

	systicks = 0;
	p_serial_writestring("\nTIME INIT\n");

	// pbyteout(0x43, 0b00110010);             
 //    pbyteout(0x40, 0x55);   			/* Set low byte of divisor */
 //    pbyteout(0x40, 0x02);    			 /* Set high byte of divisor */
	// systicks = 0;
	// p_serial_writestring("\nTIME INIT\n");
}

unsigned int time_getsystime()
{
	unsigned int ret = (unsigned int) (systicks * (float) (TIMER_MILLISECONDS_PER_TICK));
	return ret;
}

unsigned long long time_getsysticks()
{
	return systicks;
}

unsigned char get_RTC_register(int reg)
{
	pbyteout(0x70, reg);
	return pbytein(0x71);
}

int get_update_in_progress_flag()
{
	pbyteout(0x70, 0x0A);
	return (pbytein(0x71) & 0x80);
}

unsigned long long time_getCMOSTime()
{
	// pbyteout(0x70, (0x00));
	// unsigned char sec = pbytein(0x71);

	// pbyteout(0x70, (0x02));
	// unsigned char min = pbytein(0x71);

	// pbyteout(0x70, (0x04));
	// unsigned char hours = pbytein(0x71);

	// pbyteout(0x70, (0x07));
	// unsigned char days = pbytein(0x71);

	// pbyteout(0x70, (0x08));
	// unsigned char months = pbytein(0x71);

	// pbyteout(0x70, (0x09));
	// unsigned char years = pbytein(0x71);

	// pbyteout(0x70, 0x0B);
	// unsigned char regB = pbytein(0x71);

	// if(!(regB & 0x04))
	// {
	// 	p_serial_writestring("REG B SET, CONVERT TO BIN VALUES\n");
	// 	sec = (sec & 0x0F) + ((sec / 16) * 10);
	// 	min = (min & 0x0F) + ((min / 16) * 10);
	// 	hours = ((hours & 0x0F) + (((hours & 0x70) / 16) * 10)) | (hours & 0x80);
	// 	days = (days & 0x0F) + ((days /16) * 10);
	// 	months = (months & 0x0F) + ((months / 16) + 10);
	// 	years = (years & 0x0F) + ((years / 16) + 10);

	// 	p_serial_writestring("\n\n");
	// 	p_serial_writenum(sec);
	// 	p_serial_writestring(" : ");
	// 	p_serial_writenum(min);
	// 	p_serial_writestring(" : ");
	// 	p_serial_writenum(hours);
	// 	p_serial_writestring(" : ");
	// 	p_serial_writenum(days);
	// 	p_serial_writestring(" : ");
	// 	p_serial_writenum(months);
	// 	p_serial_writestring(" : ");
	// 	p_serial_writenum(years);
	// 	p_serial_writestring("\n\n");
	// }

	// return 31557600*years + 2629800*months + 87660*days + 3652*hours + 60*min + sec;

	unsigned char last_second;
	unsigned char last_minute;
	unsigned char last_hour;
	unsigned char last_day;
	unsigned char last_month;
	unsigned char last_year;
	unsigned char registerB;

	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned int year;

	// Note: This uses the "read registers until you get the same values twice in a row" technique
	//       to avoid getting dodgy/inconsistent values due to RTC updates

	while (get_update_in_progress_flag());
	                // Make sure an update isn't in progress
	second = get_RTC_register(0x00);
	minute = get_RTC_register(0x02);
	hour = get_RTC_register(0x04);
	day = get_RTC_register(0x07);
	month = get_RTC_register(0x08);
	year = get_RTC_register(0x09);

	do 
	{
		last_second = second;
		last_minute = minute;
		last_hour = hour;
		last_day = day;
		last_month = month;
		last_year = year;

		while (get_update_in_progress_flag());           // Make sure an update isn't in progress
		second = get_RTC_register(0x00);
		minute = get_RTC_register(0x02);
		hour = get_RTC_register(0x04);
		day = get_RTC_register(0x07);
		month = get_RTC_register(0x08);
		year = get_RTC_register(0x09);

	} 
	while((last_second != second) || (last_minute != minute) || (last_hour != hour) ||
	(last_day != day) || (last_month != month) || (last_year != year));

	registerB = get_RTC_register(0x0B);

	// Convert BCD to binary values if necessary

	if (!(registerB & 0x04)) 
	{
		second = (second & 0x0F) + ((second / 16) * 10);
		minute = (minute & 0x0F) + ((minute / 16) * 10);
		hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
		day = (day & 0x0F) + ((day / 16) * 10);
		month = (month & 0x0F) + ((month / 16) * 10);
		year = (year & 0x0F) + ((year / 16) * 10);
	}

	// Convert 12 hour clock to 24 hour clock if necessary

	if (!(registerB & 0x02) && (hour & 0x80)) 
	{
		hour = ((hour & 0x7F) + 12) % 24;
	}

	// Calculate the full (4-digit) year
	year += (2020 / 100) * 100;
	if(year < 2020) 
		year += 100;

	// p_serial_writestring("\n\n");
	// p_serial_writenum(second);
	// p_serial_writestring(" : ");
	// p_serial_writenum(minute);
	// p_serial_writestring(" : ");
	// p_serial_writenum(hour);
	// p_serial_writestring(" : ");
	// p_serial_writenum(day);
	// p_serial_writestring(" : ");
	// p_serial_writenum(month);
	// p_serial_writestring(" : ");
	// p_serial_writenum(year);
	// p_serial_writestring("\n\n");

	unsigned long long posixTime = second + (60*minute) + (3600*hour) + (86400 * day) + (2678400 * month);
  
  	return posixTime;
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
	timer_wait(ms*2);
}