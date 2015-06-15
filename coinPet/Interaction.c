#include "interaction.h"

void init_interaction(void)
{
    // for led interacition
    DDRD  |= (1<<RED_LED)|(1<<BLUE_LED)|(1<<GREEN_LED);
    PORTD |= (1<<RED_LED)|(1<<BLUE_LED)|(1<<GREEN_LED);

    // for melody
    DDRA |= (1<<MELODY);
}

void melody_interaction()
{
    PORTA |= (1<<MELODY);
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

