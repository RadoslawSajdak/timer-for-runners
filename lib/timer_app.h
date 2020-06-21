/************************************************************************/
/* Author: ---
/************************************************************************/
#include "avr/io.h"
#include "LCD.h"


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
#define TIMER_SCREEN	0
#define LAP1_SCREEN		1
#define LAP2_SCREEN		2

#define SPEED			36
#define STEP			50

struct timer_lap
{
	uint8_t		milis;
	uint8_t		seconds;
	uint8_t		minutes;
	uint8_t		hours;
	uint16_t	distance;
	uint8_t		state;
	
};

struct timer_bank
{
	
	struct timer_lap lap1;
	struct timer_lap lap2;
	struct timer_lap lap3;
	struct timer_lap setup;
}; 

void timer_app_init();

/************************************************************************/
/*@brief	Function showing time in HH:MM:SS:mm format at LCD
/*
/*@param[in]	Timer_lap structure including HH,MM,SS,mm
/*@param[in]	uint8_t 0-1 rows on 16x2 LCD
/*@param[in]	uint8_t 0-5 starting column
/*
/*@return[out]	uint8_t actual milis clock
/************************************************************************/
void timer_show_time(struct timer_lap * time, uint8_t row, uint8_t column);

/************************************************************************/
/*@brief	Simple function for timer reset. It put zeros to HH,MM,SS,mm
/*
/*@param[in]	pointer to struct with values to reset
/************************************************************************/
void timer_reset(struct timer_lap * to_reset);

/************************************************************************/
/*@brief	Function to find out which button was pressed. Don't forget
/*			to protect this function with sei() anc cli()
/*
/*@return	number of pressed button
/************************************************************************/
uint8_t timer_button_pressed();

void timer_display(uint8_t button, uint8_t * screen_num, struct timer_lap * state, uint8_t * position);

/************************************************************************/
/*@brief	Function used to simulate distance. Value of distance is 
/*			calculated from pre-defined values.
/*
/*@param[in]	Current value of time
/*@return	Int value of distance with not less than 10m step
/************************************************************************/
void distance_simulator(struct timer_lap * sim_time);

void distance_check(struct timer_lap * current_time, struct timer_bank * setup_time, uint8_t * check);

void setup_interval(struct timer_bank * to_setup);