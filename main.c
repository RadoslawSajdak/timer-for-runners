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

struct timer_bank first_interval;
struct timer_bank second_interval;
struct timer_lap actual_time;


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
	
	lcd_write_text("HELLO");
	/* ADC converter setup */
	ADCSRA =  (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
	ADMUX  =  (1<<REFS1) | (1<<REFS0) | (1<<MUX1) ; 
	DDRC &= 0b11100011;
	_delay_ms(1000);
	
	/* Watchdog setup */
	cli();
	MCUSR = 0x00;
	WDTCSR = (1<<WDCE)|(1<<WDE);
	WDTCSR = (1<<WDIE) | (1<<WDP0) ; //32ms prescaler 
	sei();


    while (1) 
    {
		asm("nop");
    }
	
	
}

ISR(WDT_vect)
{
	cli();
	screen ++;
	if ((screen % 3) == 0)
	{
		_delay_ms(4); // We need it to compensate prescaler error 3*32ms = 96, not 100ms
		screen = timer_show_time(&actual_time,1,2);
	}
	WDTCSR |= (1<<WDIE);
	sei();
}

ISR(INT0_vect)
{
	cli();
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
		
	if ((ADC > 120) && (ADC < 150))
	{
		lcd_set_position(0,0);
		lcd_write_text("Button0");
	}
	if ((ADC > 155) && (ADC < 180))
	{
		lcd_set_position(0,0);
		lcd_write_text("Button1");
	}
	if ((ADC > 350) && (ADC < 500))
	{
		lcd_set_position(0,0);
		lcd_write_text("Button2");
	}
	printf("%d\r\n",ADC);
	_delay_ms(400);
	sei();
}