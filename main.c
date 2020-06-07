/*
 * Timer for runners.c
 *
 * Created: 2020-06-06 11:25:22
 * Author : sajdak
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr/interrupt.h"
#include "LCD.h"
#include "timer_app.h"

uint8_t * screen = 0;

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
	
	_delay_ms(1000);
	sei();

    while (1) 
    {
		asm("nop");
    }
}

ISR(INT0_vect)
{
	cli();
	uint8_t test_button = BUTTON_PIN;
	printf("INTERRUPT! 0x%x\n\r",test_button);
	if (test_button & BUTTON0_MASK)
	{
		lcd_set_position(0,0);
		lcd_write_text("Button0");
	}
	else if (test_button & BUTTON1_MASK)
	{
		lcd_set_position(0,0);
		lcd_write_text("Button1");
	}
	else if (test_button & BUTTON2_MASK)
	{
		lcd_set_position(0,0);
		lcd_write_text("Button2");
	}

	
	_delay_ms(400);
	sei();
}