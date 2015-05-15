#include "interaction.h"

void init_interaction()
{
    DDRD |= (1<<COIN_LED);
}

void led_interaction(unsigned char time, unsigned int speed, unsigned int threshold)
{
    int i = 0;
    int j = 0;
    
    unsigned char flag = 0;
    
    for(j=0;j<time;j++);
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
    }
}

