#include "interaction.h"

void init_interaction(void)
{
    DDRD |= (1<<COIN_LED);
}

void led_interaction(unsigned int time, unsigned int speed, unsigned int threshold)
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
                PORTD |= (1<<COIN_LED);
            else
                PORTD &= ~(1<<COIN_LED);
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

