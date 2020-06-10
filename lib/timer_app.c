#include "timer_app.h"

void timer_app_init()
{
	BUTTON_DDR |= (1 << BUTTON2) | (1 << BUTTON1) | (1 << BUTTON0);

}


void timer_show_time(struct timer_lap * time, uint8_t row, uint8_t column)
{
	
	if (time->milis >= 10)
	{
		time->seconds ++;
		time->milis = 0;
	}
	if (time->seconds >= 60)
	{
		time->minutes ++;
		time->seconds = 0;
	}
	if (time->minutes >= 60)
	{
		time->hours ++;
		time->minutes = 0;
	}
	if (time->hours >= 100)
	{
		time->hours = 0;
	}
		
	lcd_set_position(row,column);
	lcd_write_int(time->hours);
	lcd_write_text(":");
	lcd_write_int(time->minutes);
	lcd_write_text(":");
	lcd_write_int(time->seconds);
	lcd_write_text(":");
	lcd_write_int(time->milis*10);

}

void timer_reset(struct timer_lap * to_reset)
{
	to_reset->hours = 0;
	to_reset->minutes = 0;
	to_reset->seconds = 0;
	to_reset->milis = 0;
}