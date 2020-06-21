/*
 * LCD library
 * Created: 04.2020
 * Author : Mateusz Kozyra
 * Purpose : Part of project made at AGH UST.
 * Description : This library contain all necessary function used to control LCD display 
 *					we need 6 connection ports (4 for data, 2 for control)
 * Features : LCD initialization, defined set of command to use, Easy string and float presentation on LCD,
 *				we could cheate custom char
 */ 




#ifndef LCD_H_
#define LCD_H_

#define F_CPU 16000000UL//16 MHz
#include <util/delay.h>#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
//#include "TWI.h"
//#include "DHT11.h"
#include <stdbool.h>
#include <avr/interrupt.h>


/************************************************************************
 @brief	Set or clear single bit in register

 @param[in]	PORT - register localization
 @param[in]	PIN - bit number
************************************************************************/
#define SET_PORT_PIN(PORT, PIN)		PORT|=(1<<PIN)
#define CLEAR_PORT_PIN(PORT, PIN)	PORT&= (~(1<<PIN))


/*******************************LCD CONECTION****************************/
#define LCD_RS_PIN		0
#define LCD_RS_PORT		PORTC
#define LCD_RS_DDR		DDRC
#define LCD_E_PIN		1
#define LCD_E_PORT		PORTC
#define LCD_E_DDR		DDRC
#define LCD_D7_PIN		7
#define LCD_D6_PIN		6
#define LCD_D5_PIN		5
#define LCD_D4_PIN		4
#define LCD_DATA_PORT	PORTD
#define LCD_DATA_DDR	DDRD
/***********************************************************************/

/*************************SET/CLEAR CONTROL PORT************************/
#define LCD_SET_E		SET_PORT_PIN(LCD_E_PORT, LCD_E_PIN)				//1-START TRANSMISION 0-STOP
#define LCD_CLEAR_E		CLEAR_PORT_PIN(LCD_E_PORT, LCD_E_PIN)
#define LCD_SET_RS		SET_PORT_PIN(LCD_RS_PORT, LCD_RS_PIN)			//0-INSTRUCTION	1-DATA
#define LCD_CLEAR_RS	CLEAR_PORT_PIN(LCD_RS_PORT, LCD_RS_PIN)
/***********************************************************************/

/**************************LCD INSTRUCTIONS*****************************/
#define LCD_DISPLAY_CLEAR		0X01
#define LCD_CURSOR_HOME			0X02
#define LCD_ENTRY_MODE_SET		0X04
#define LCD_EMS_STABLE_CURSOR	0X01
#define LCD_EMS_DYNAMIC_CURSOR	0X00
#define LCD_EMS_INC_ADDR		0X02
#define LCD_EMS_DEC_ADDR		0X00
#define LCD_VISIBLE				0X08
#define LCD_V_DISPLAY_ON		0X04
#define LCD_V_DISPLAY_OFF		0X00
#define LCD_V_CURSOR_ON			0X02
#define LCD_V_CURSOR_OFF		0X00
#define LCD_V_BLINK_ON			0X01
#define LCD_V_BLINK_OFF			0X00
#define LCD_SHIFT				0X10
#define LCD_S_WINDOW			0X08
#define	LCD_S_CURSOR			0X00
#define LCD_S_RIGHT				0X04
#define LCD_S_LEFT				0X00
#define LCD_FUNCTION_SET		0X20
#define LCD_FS_8BIT				0X10
#define LCD_FS_4BIT				0X00
#define LCD_FS_2ROW				0X08
#define LCD_FS_1ROW				0X00
#define LCD_FS_5X10CHAR			0X04
#define LCD_FS_5X7CHAR			0X00
#define LCD_CG_RAM_SET			0X40
#define LCD_DD_RAM_SET			0X80


/************************************************************************
 @brief Init LCD display module
			configure pinout, set communication type, clear display
************************************************************************/
void lcd_init();

/************************************************************************
 @brief Send 4 bits to LCD - 4bit communication only

 @param[in] bits - data or instruction
************************************************************************/
void lcd_write_4bit(uint8_t bits);

/************************************************************************
 @brief Send 8 bits to LCD (2x4bit) - 4bit communication only

 @param[in] bits - data or instruction
************************************************************************/
void lcd_write_8bit(uint8_t bits);

/************************************************************************
 @brief Send 8 bits of instruction (from LCD INSTRUCTIONS' list)

 @param[in] defined bits
************************************************************************/
void lcd_write_instruction(uint8_t bits);

/************************************************************************
 @brief Send 8 bit of data

 @param[in] code of char
************************************************************************/
void lcd_write_data(uint8_t bits);

/************************************************************************
 @brief Display string at LCD

 @param[in] String or char pointer
************************************************************************/
void lcd_write_text(char *text);

/************************************************************************
 @brief Set cursor position

 @param[in] row {0,1} and collumn   (0,0)-upper left corner
************************************************************************/
void lcd_set_position(uint8_t row , uint8_t collumn);

/************************************************************************
 @brief Create custom char

 @param[in] Array of hex values -size[8]!!!

 @return char address in memory - use in lcd_write_data function
************************************************************************/
uint8_t lcd_create_char(uint8_t *character);

/************************************************************************
 @brief Print int

 @param[in] Value to print
************************************************************************/
void lcd_write_int(int value);

/************************************************************************
 @brief Print float value on LCD - up to 1/100

 @param[in] value to display
************************************************************************/
void lcd_write_float(float value);

/************************************************************************
 @brief Print sensor value

 @param[in] temperature outside/inside humidity outside/inside in struct
************************************************************************/
//void lcd_display_sensor (struct Sensor *inside, struct Sensor *outside);

/************************************************************************
 @brief Specialized function - if one sample is held in two int-variables

 @param[in] int part and fractal part
************************************************************************/
void lcd_write_sample (uint8_t value_int, uint8_t value_fractal);


#endif  /*LCD_H_*/ 