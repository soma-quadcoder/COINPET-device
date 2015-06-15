#include "interaction.h"
#include <avr/interrupt.h>

ISR(TIMER1_OVF_vect)
{
    static unsigned char cnt = 0;
    
    TCNT1 = 0x00;
    cnt++;
    
    // 6초가 지난경우에 멜로디를 끔
    if( cnt==2 )
    {
        PORTA  &= ~(1<<MELODY);
        
        // 타이머1 해제
        TIMSK1 = 0x00;
        cnt = 0;
    }
}

void init_interaction(void)
{
    // for led interacition
    DDRD   |= (1<<RED_LED)|(1<<BLUE_LED)|(1<<GREEN_LED);
    PORTD  |= (1<<RED_LED)|(1<<BLUE_LED)|(1<<GREEN_LED);
    
    // for melody
    DDRA  |= (1<<MELODY);
    
    // 멜로디를 위한 타이머1 세팅
    TCCR1B |= (1<<CS12)|(1<<CS10);
    
}

void melody_interaction()
{
    // 타이머 1 실행
    cli();
    
    PORTA  |= (1<<MELODY);
    
    // 멜로디 출력시간을 위한 타이머 1 카운팅
    TCNT1   = 0x00;

    TIMSK0  = 0x00;
    TIMSK1 |= (1<<TOIE1);
    
    sei();
}

void led_interaction(unsigned color, unsigned int time, unsigned int speed, unsigned int threshold)
{
    /*
     time       : led깜빡이는 횟수 지정
     speed      : led 최대로 밝아질때까지의 속도 지정
     threshold  : led 최대 밝기 지정
     */
     
    int i = 0;
    int j = 0;
    
    unsigned char flag = 0;
    
    while(j!= (time*5))
    {
        for(i=0;i<speed;i++)
        {
            if(i<threshold)
                PORTD &= ~(1<<color);
            else
                PORTD |= (1<<color);
        }
        if(threshold==speed)
            flag=1;
        else if(threshold==0)
            flag=0;
        
        if(flag)
            threshold--;
        else
            threshold++;
        j++;
    }
}

