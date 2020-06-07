#include "timer_app.h"

void timer_app_init()
{
	BUTTON_DDR |= (1 << BUTTON2) | (1 << BUTTON1) | (1 << BUTTON0);
	//BUTTON_PORT |= (1 << BUTTON2) | (1 << BUTTON1) | (1 << BUTTON0);
}