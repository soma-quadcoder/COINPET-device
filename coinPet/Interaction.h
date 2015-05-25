#include<avr/io.h>
#define F_CPU 20000000
#include<util/delay.h>

#define COIN_LED PD7

void led_interaction(unsigned int time, unsigned int speed, unsigned int threshold);
void init_interaction(void);
