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

int screen = 0;

void init_UART(void)
{
	//Set baud rate 115200
	UBRR0H = 0;
	UBRR0L = 16;
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	//Double Clock Speed
	UCSR0A = (1<<U2X0);
	// Set frame format: 8data, 1stop bit
	UCSR0C = (3<<UCSZ00); //(1<<USBS0)|
}
int uart_putchar(char c, FILE *stream){
	//wait until buffer empty
	while ( !( UCSR0A & (1<<UDRE0)) );
	//Put data into buffer
	UDR0 = c;
	
	return 0;
}

/* This structures will be used in interrupt so it must be globally*/
//struct timer_bank intervals[0];
//struct timer_bank intervals[1];
struct timer_bank intervals[2];
struct timer_lap actual_time;
uint8_t actual_screen = TIMER_SCREEN;
uint8_t position = 0;
int main(void)
{
	init_UART();
	FILE str_uart = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &str_uart;
	
	
	
	
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
	
	intervals[0].setup.distance = 50;
	intervals[1].setup.distance = 0;
    while (1) 
    {
		switch (actual_screen)
		{
			/* Timer screen */
			case TIMER_SCREEN:
			{
				
				if (actual_time.state == 0)
				{
					distance_simulator(&actual_time);
					WDTCSR &= (~(1 << WDIE));
					timer_show_time(&actual_time,1,2);
					_delay_ms(100);
					
				}
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
				if (actual_time.state & 0x02)
				{
					actual_time.state &= 0xfd;
					setup_interval(&intervals[0]);
				}
				break;
			}
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
	uint8_t button = timer_button_pressed();
	timer_display(button, &actual_screen, &actual_time,&position );
	_delay_ms(400);
	sei();
}