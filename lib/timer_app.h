/************************************************************************/
/* Author: ---
/************************************************************************/
#include "avr/io.h"



#ifndef cbi
#define cbi(sfr, bit)     (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit)     (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#define BUTTON_DDR		DDRB
#define BUTTON_PORT		PORTB
#define BUTTON_PIN		PINB
#define BUTTON0			0
#define BUTTON1			1
#define BUTTON2			2
#define BUTTON0_MASK	0x01
#define BUTTON1_MASK	0x02
#define BUTTON2_MASK	0x04

void timer_app_init();