/************************************************************************/
/* Author: Sajdak Radoslaw
/************************************************************************/
#include "avr/io.h"
#include "LCD.h"

/*	If you're not sure what are you doing, try to not change this
	values and connect your arduino with it */
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

/* This values are for user. Feel free to change it */
#define SPEED			20	// kmph. *1000/3600=m/s
#define STEP			50	// step in interval settings
#define MAX_INTERVAL	300
#define ISDT				1500 //interval setup display time in milis

/*	Basic structures for this app. It's used to store all\
	data */
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
/************************************************************************/
/*@brief	Initialize function. To be honest it only set pin directions
/************************************************************************/
void timer_app_init();

/************************************************************************/
/*@brief	Function showing time in HH:MM:SS:mm format at LCD
/*
/*@param[in]	Timer_lap structure including HH,MM,SS,mm
/*@param[in]	uint8_t 0-1 rows on 16x2 LCD
/*@param[in]	uint8_t 0-5 starting column
/*
/************************************************************************/
void timer_show_time(struct timer_lap * time, uint8_t row, uint8_t column);

/************************************************************************/
/*@brief	Simple function for timer reset. It put zeros to HH,MM,SS,mm
/*			It will reset current distance too!
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

/************************************************************************/
/*@brief	Function to change displayed screen
/*
/*@param[in]	Button number. Usually it's returned value of
/*				timer_button_pressed()
/*@param[in]	Pointer to current screen number value. It will be changed
/*				if we do it with button
/*@param[in]	Pointer to struct of actual time. It will change state 
/*				value in specified situations and tell
/*@param[in]	Pointer to position. It's using for interval-list
/************************************************************************/
void timer_display(uint8_t button, uint8_t * screen_num, struct timer_lap * state, uint8_t * position);

/************************************************************************/
/*@brief	Function used to simulate distance. Value of distance is 
/*			calculated from predefined values.
/*
/*@param[in]	Structure pointer to current time. It will change distance
/*				value in input structure
/************************************************************************/
void distance_simulator(struct timer_lap * sim_time);

/************************************************************************/
/*@brief	Function to compare current distance and distance set in lap
/*			setup. If it will be almost equal, function will save times
/*			in second structure
/*
/*@param[in]	Structure pointer to current time
/*@param[in]	Structure pointer to bank with 3 laps for interval storing
/************************************************************************/
void distance_check(struct timer_lap * current_time, struct timer_bank * setup_time);

/************************************************************************/
/*@brief	Function to setup interval with button 0. It will display
/*			value for ISDT milis
/*
/*@param[in]	structure pointer. It will change setup distance value
/************************************************************************/
void setup_interval(struct timer_bank * to_setup);
