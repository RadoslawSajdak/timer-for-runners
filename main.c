/*
 * Timer for runners.c
 *
 * Created: 2020-06-06 11:25:22
 * Author : sajdak
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include "avr/wdt.h"
#include "avr/interrupt.h"
#include "LCD.h"
#include "timer_app.h"



/* This structures will be used in interrupt so it must be globally*/
struct timer_bank intervals[2];
struct timer_lap actual_time;
uint8_t actual_screen = TIMER_SCREEN;
uint8_t position = 0;
uint8_t screen = 0;
int main(void)
{
	lcd_init();
	timer_app_init();
	/* Interrupts setup */
	PORTD |= (1 << PIND2); //using pullup resistor
	EICRA = (1 << ISC01) | (0 << ISC00);
	EIMSK = (1 << INT0);
	EIFR  = (1 << INTF1) | (1 << INTF0);
	
	/* ADC converter setup */
	ADCSRA =  (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	ADMUX  =  (1 << REFS1) | (1 << REFS0) | (1 << MUX1) ; 
	DDRC &= 0b11100011;
	_delay_ms(1000);
	
	/* Watchdog setup */
	cli();
	MCUSR = 0x00;
	WDTCSR = (1 << WDCE) | (1 << WDE);
	WDTCSR = (1 << WDP0) ; //32ms prescaler 
	sei();
	/* Default values for intervals */
	intervals[0].setup.distance = 50;
	intervals[1].setup.distance = 0;
	
	/* Main program */
    while (1) 
    {
		/* We're going to switch between next screens in loop */
		switch (actual_screen)
		{
			/* Timer screen */
			case TIMER_SCREEN:
			{
				/* If state is 0, timer is off so we just show actual values */
				if (actual_time.state == 0)
				{
					distance_simulator(&actual_time);
					WDTCSR &= (~(1 << WDIE));
					timer_show_time(&actual_time,1,2);
					_delay_ms(100);
					
				}
				/* If state is 1, timer is run so we have to compare current distance to required intervals */
				if (actual_time.state == 1)
				{
					WDTCSR |= (1 << WDIE);
					distance_simulator(&actual_time);
					for (int i = 0; i < 2; i++)
					{
						distance_check(&actual_time,&intervals[i]);
					}
					_delay_ms(100);
				}
				
				break;
			}
			/*	Display first "bank" with our times in intervals. Position is used to choose\
				which pair of times we're going to show. Position helps to simulate "list move"\
				1->2->3->1 etc.*/
			case LAP1_SCREEN:
			{
				if (position%2 != 2)
				{
					timer_show_time(&intervals[0].lap1, position%2, 0);
					_delay_ms(100);
					lcd_set_position(position%2,11);
					lcd_write_text(" LAP1");
					_delay_ms(100);
				}
				if (position%2 + 1 != 2)
				{
					timer_show_time(&intervals[0].lap2, position%2 + 1 , 0);
					_delay_ms(100);
					lcd_set_position(position%2 + 1,11);
					lcd_write_text(" LAP2");
					_delay_ms(100);
				}
				if (position%2 + 2 != 2)
				{
					timer_show_time(&intervals[0].lap3, 0 , 0);
					_delay_ms(100);
					lcd_set_position(0,11);
					lcd_write_text(" LAP3");
					_delay_ms(100);
				}
				/* We're using second bit in state to change interval. Because\
					We don't want to change any other bit in state, we mask it*/
				if (actual_time.state & 0x02)
				{
					actual_time.state &= 0xfd;
					setup_interval(&intervals[0]);
				}
				break;
			}
			/* Lap 2 works exactly the same to lap 1 */
			case LAP2_SCREEN:
			{
				if (position%2 != 2)
				{
					timer_show_time(&intervals[1].lap1, position%2, 0);
					_delay_ms(100);
					lcd_set_position(position%2,11);
					lcd_write_text(" LAP1");
					_delay_ms(100);
				}
				if (position%2 + 1 != 2)
				{
					timer_show_time(&intervals[1].lap2, position%2 + 1 , 0);
					_delay_ms(100);
					lcd_set_position(position%2 + 1,11);
					lcd_write_text(" LAP2");
					_delay_ms(100);
				}
				if (position%2 + 2 != 2)
				{
					timer_show_time(&intervals[1].lap3, 0 , 0);
					_delay_ms(100);
					lcd_set_position(0,11);
					lcd_write_text(" LAP3");
					_delay_ms(100);
				}
				if (actual_time.state & 0x02)
				{
					actual_time.state &= 0xfd;
					setup_interval(&intervals[1]);
				}
				break;
			}
			
			
		}
    }
	
	
}

ISR(WDT_vect)
{
	cli();
	/* Internal clock isn't linear. Because we want 100ms step\
		we need to interrupt 3 times after 32ms. Screen variable\
		is used to remember value permanently */
	screen ++;
	if ((screen % 3) == 0)
	{
		_delay_ms(4); // We need it to compensate prescaler error 3*32ms = 96, not 100ms
		actual_time.milis++;
		timer_show_time(&actual_time,1,2);
	}
	WDTCSR |= (1 << WDIE);
	sei();
}

ISR(INT0_vect)
{
	cli();
	/*	Because Atmega328p has only 2 external interrupts we used\
		ADC converter. We're checking which button was pressed */
	uint8_t button = timer_button_pressed();
	/*	Function to change displayed informations */
	timer_display(button, &actual_screen, &actual_time,&position );
	/*	We need this delay to compensate bouncing and LCD delays */
	_delay_ms(400);
	sei();
}