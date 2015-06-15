#include<avr/io.h>
#define F_CPU 20000000
#include<util/delay.h>

#define BLUE_LED     PD7
#define GREEN_LED    PD6
#define RED_LED      PD5

#define MELODY       PA2

void led_interaction(unsigned color,unsigned int time, unsigned int speed, unsigned int threshold);
void melody_interaction();
void init_interaction(void);
