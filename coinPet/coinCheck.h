#include <avr/io.h>
#include <util/delay.h>


//------ 동전인식을 위한 상수 선언 ---------------
#define         SHIPWON             10
#define         OHSHIPWON           50
#define         BACKWON             100
#define         OHBACKWON           500
//------------------------------------------

#define         COINDETEC_PIN       PB0

//---- 동전구별에 사용될 전역변수 선언 -------------
unsigned int    adc_max;
unsigned long   current_money;
unsigned long   goal_money;

unsigned char   data_unconnect[4];
//------------------------------------------

void init_adc();
void proccess_coin();