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
/*	We need to check range because ACD measure isn't perfect */
uint8_t timer_button_pressed()
{
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	
	if ((ADC > 120) && (ADC < 150))
	{
		return BUTTON0;
	}
	else if ((ADC > 155) && (ADC < 180))
	{
		return BUTTON1;
	}
	else if ((ADC > 350) && (ADC < 500))
	{
		lcd_write_instruction(LCD_DISPLAY_CLEAR);
		return BUTTON2;
	}
	return -1;
}

void timer_display(uint8_t button, uint8_t * screen_num, struct timer_lap * state, uint8_t * position)
{
	if( * screen_num == TIMER_SCREEN && state->state == 1)
	{
		if (button == BUTTON0)
		{
			state->state ^= 0x01;	//It will change first bit of state - run/stop timer
		}
	}
	else if (* screen_num == TIMER_SCREEN && state->state == 0)
	{
		if (button == BUTTON0)
		{
			state->state ^= 0x01;
			* screen_num = TIMER_SCREEN;
		}
		else if (button == BUTTON1)
		{
			timer_reset(state);		//on timer screen, Button 1 is used to reset
		}
		else if (button == BUTTON2)
		{
			lcd_write_instruction(LCD_DISPLAY_CLEAR);
			
			* screen_num = LAP1_SCREEN;	//Button 2 will switch displayed screen
		}
	}
	else if (* screen_num == LAP1_SCREEN)
	{
		if (button == BUTTON0)
		{
			state->state |= 0x02;
		}
		else if (button == BUTTON1)
		{
			* position += 1;		//Button1 will increment position val to simulate list effect
		}
		else if (button == BUTTON2)
		{
			lcd_write_instruction(LCD_DISPLAY_CLEAR);
			
			* screen_num = LAP2_SCREEN;
		}
	}
	else if (* screen_num == LAP2_SCREEN)
	{
		if (button == BUTTON0)
		{
			state->state |= 0x02;
		}
		else if (button == BUTTON1)
		{
			* position += 1;		//Button1 will increment position val to simulate list effect
		}
		else if (button == BUTTON2)
		{
			lcd_write_instruction(LCD_DISPLAY_CLEAR);
			_delay_ms(400);
			* screen_num = TIMER_SCREEN; 
			state->state = 0;
			
		}
	}
}
void distance_simulator(struct timer_lap * sim_time)
{
	cli();
	unsigned int sim_seconds = sim_time->hours*3600 + sim_time->minutes*60 + sim_time->seconds; // recalculate to seconds
	float distance = SPEED * (sim_seconds + sim_time->milis/100) * 0.28; //recalculate to m/s and then calculate distance

	sim_time->distance = (int)distance;
	lcd_set_position(0,0);
	lcd_write_text("Distance:      "); //We need to clear line
	lcd_set_position(0,9);
	lcd_write_int(sim_time->distance);
	sei();
}

void distance_check(struct timer_lap * current_time, struct timer_bank * setup_time)
{
	/************************************************************************/
	/*	It's int because we don't care about decimal values. It will show us
	/*	how much times current time is bigger than set and then we'll be able
	/*	to calculate which time we're crossing lap.
	/************************************************************************/
	int n = current_time->distance / setup_time->setup.distance; 
	/************************************************************************/
	/*	Here we used here 5m buffer because rounding up in calculations can
	/*	add small errors so we'll not always be able to hit exactly 100m for
	/*	example
	/************************************************************************/
	if (( current_time->distance >= setup_time->setup.distance * n) && (current_time->distance < (setup_time->setup.distance + 5) * n))
	{
		if(n % 3 == 1) setup_time->lap1 = * current_time;
		if(n % 3 == 2) setup_time->lap2 = * current_time;
		if(n % 3 == 0) setup_time->lap3 = * current_time;
		
	}
}

void setup_interval(struct timer_bank * to_setup)
{
	lcd_write_instruction(LCD_DISPLAY_CLEAR);
	_delay_ms(50);
	lcd_set_position(0,0);
	lcd_write_text("Interval:");
	to_setup->setup.distance += STEP;
	if(to_setup->setup.distance > MAX_INTERVAL) to_setup->setup.distance = 0;
	lcd_write_int(to_setup->setup.distance);
	_delay_ms(ISDT);
	lcd_write_instruction(LCD_DISPLAY_CLEAR);
	
}