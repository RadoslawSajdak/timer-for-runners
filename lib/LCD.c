#include "LCD.h"

void lcd_init(){
	
	LCD_DATA_DDR |= (1<<LCD_D7_PIN) | (1<<LCD_D6_PIN) | (1<<LCD_D5_PIN) | (1<<LCD_D4_PIN);
	LCD_E_DDR |= (1<<LCD_E_PIN);				//PINOUT SETUP
	LCD_RS_DDR |= (1<<LCD_RS_PIN);
	_delay_ms(50);
	
	/**********************************DATASHEET PROCEDURE*********************************/
	LCD_CLEAR_E;
	LCD_CLEAR_RS;
	
	
	lcd_write_4bit(0x03);
	_delay_ms(5);
	
	lcd_write_4bit(0x03);
	_delay_us(120);
	
	lcd_write_4bit(0x03);
	_delay_us(120);
	
	lcd_write_4bit(0x02);
	_delay_us(120);
	
	lcd_write_instruction(LCD_FUNCTION_SET | LCD_FS_4BIT | LCD_FS_2ROW | LCD_FS_5X7CHAR);
	lcd_write_instruction(LCD_VISIBLE);
	lcd_write_instruction(LCD_DISPLAY_CLEAR);
	_delay_ms(10);
	
	/*********************************USER CONFIGURATION************************************/
	lcd_write_instruction(LCD_ENTRY_MODE_SET | LCD_EMS_DYNAMIC_CURSOR | LCD_EMS_INC_ADDR);
	lcd_write_instruction(LCD_CURSOR_HOME);
	_delay_ms(2);
	lcd_write_instruction(LCD_VISIBLE | LCD_V_DISPLAY_ON | LCD_V_CURSOR_OFF);
	

}

void lcd_write_4bit(uint8_t bits){
	LCD_SET_E;				//rising edge for transmission beginning	
	if (bits & 0x01) SET_PORT_PIN(LCD_DATA_PORT,LCD_D4_PIN); else CLEAR_PORT_PIN(LCD_DATA_PORT,LCD_D4_PIN);	
	if (bits & 0x02) SET_PORT_PIN(LCD_DATA_PORT,LCD_D5_PIN); else CLEAR_PORT_PIN(LCD_DATA_PORT,LCD_D5_PIN);
	if (bits & 0x04) SET_PORT_PIN(LCD_DATA_PORT,LCD_D6_PIN); else CLEAR_PORT_PIN(LCD_DATA_PORT,LCD_D6_PIN);
	if (bits & 0x08) SET_PORT_PIN(LCD_DATA_PORT,LCD_D7_PIN); else CLEAR_PORT_PIN(LCD_DATA_PORT,LCD_D7_PIN);
	LCD_CLEAR_E;			//falling edge for transmission end
}

void lcd_write_8bit(uint8_t bits){
	lcd_write_4bit(bits>>4);
	lcd_write_4bit(bits);
	_delay_us(50);
}

void lcd_write_instruction(uint8_t bits){
	LCD_CLEAR_RS;			//RS=0 ->instruction
	lcd_write_8bit(bits);
}

void lcd_write_data(uint8_t bits){
	LCD_SET_RS;				//RS=1 ->data
	lcd_write_8bit(bits);
}


void lcd_write_text(char *text)
{
	for (uint8_t m = 0; m < strlen(text); m++)
	{
		lcd_write_data(text[m]);
	}
}

void lcd_set_position(uint8_t row , uint8_t collumn){

	if (row==0)
	{
		lcd_write_instruction(LCD_DD_RAM_SET | (0x00 + collumn));  //0x00 - (r=0,c=0) position
	} 
	else if (row==1)
	{
		lcd_write_instruction(LCD_DD_RAM_SET | (0x40 + collumn)); //0x40 - (r=1,c=0) position
	}
}


volatile uint8_t icon=0;   //global variable - it count custom char in memory

uint8_t lcd_create_char(uint8_t *bit_matrix){
	lcd_write_instruction(LCD_CG_RAM_SET | icon<<3); //set proper address in memory
	for (uint8_t m = 0; m<8; m++)
	{
		lcd_write_data(*bit_matrix);
		bit_matrix++;
	}
	icon++;
	
	return icon-1;
}

void lcd_write_int(int value){
	
	char buffor[16];
	itoa(value,buffor,10);
	if (value<10)	lcd_write_text("0");
	lcd_write_text(buffor);
	
}

void lcd_write_float(float value){

	int part_int = (int)value;
	
	int part_float = (int) (value*10.0);
	part_float = abs(part_float%10);
	
	if (abs(part_int)<10)	lcd_write_text("0");
	lcd_write_int(part_int);
	
	if(part_float){
		lcd_write_text(".");
		lcd_write_int(part_float);
	}
	
}


/******************************* SPECJALIZED ********************/
/*void lcd_display_sensor (struct Sensor *inside, struct Sensor *outside){
	
	icon = 0;		//overwrite all custom icon
	uint8_t bits_drop[8] = {0x04,0x04,0x0A,0x0A,0x11,0x11,0x0E,0x00};		//store custom icon bits
	uint8_t bits_thermometer[8]={0x04,0x0A,0x0A,0x0E,0x0E,0x1F,0x1F,0x0E};	
	uint8_t bits_sun2[8]={0x1D,0x0B,0x07,0x04,0x07,0x0B,0x1D,0x1F};
	uint8_t bits_sun1[8] = {0x17,0x1A,0x1C,0x04,0x1C,0x1A,0x17,0x1F};
	uint8_t bits_house1[8] = {0x1F,0x1E,0x1C,0x18,0x13,0x18,0x18,0x1F};
	uint8_t bits_house2[8] = {0x1F,0x0F,0x07,0x03,0x19,0x03,0x03,0x1F};
		
	uint8_t icon_drop = lcd_create_char(bits_drop);
	uint8_t icon_thermo = lcd_create_char(bits_thermometer);
	uint8_t icon_sun1 = lcd_create_char(bits_sun1);
	uint8_t icon_sun2 = lcd_create_char(bits_sun2);
	uint8_t icon_house1 = lcd_create_char(bits_house1);
	uint8_t icon_house2 = lcd_create_char(bits_house2);	
	
	lcd_set_position(0,0);
	lcd_write_data(icon_thermo);
	if (inside->temp_int<0) lcd_write_text("-"); else lcd_write_text("+");
	lcd_write_sample(outside->temp_int, outside->temp_fractal);
	lcd_write_text("C");
	lcd_set_position(0,7);
	lcd_write_data(icon_sun1);
	lcd_write_data(icon_sun2);
	lcd_set_position(0,10);
	lcd_write_sample(outside->humidity_int, outside->humidity_fractal);
	lcd_write_text("%");
	lcd_write_data(icon_drop);
	
	lcd_set_position(1,0);
	
	lcd_write_data(icon_thermo);
	if (outside->temp_int<0) lcd_write_text("-"); else lcd_write_text("+");
	lcd_write_sample(inside->temp_int, inside->temp_fractal);
	lcd_write_text("C");
	lcd_set_position(1,7);
	lcd_write_data(icon_house1);
	lcd_write_data(icon_house2);
	lcd_set_position(1,10);
	lcd_write_sample(inside->humidity_int, inside->humidity_fractal);
	lcd_write_text("%");
	lcd_write_data(icon_drop);

}*/

void lcd_write_sample (uint8_t value_int, uint8_t value_fractal){
	lcd_write_int(value_int);
	lcd_write_text(".");
	lcd_write_int(value_fractal);
}
